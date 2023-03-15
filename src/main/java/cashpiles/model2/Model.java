package cashpiles.model2;

import java.time.LocalDate;
import java.time.Period;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.NavigableMap;
import java.util.Optional;
import java.util.TreeMap;
import java.util.UUID;

import cashpiles.currency.Amount;

class Model extends ModelItem {

	enum IdentifierType {
		ACCOUNT, CATEGORY, OWNER
	}

	private Map<String, Account> accounts = new TreeMap<>();
	private NavigableMap<LocalDate, BudgetPeriod> budgetPeriods = new TreeMap<>();
	private Map<String, IdentifierType> identifiers = new TreeMap<>();
	private Map<UUID, Transaction> transactions = new HashMap<>();

	/*
	 * Or.... transactions contain entries that are a polymorphic set of entry
	 * types. Each of those types can be passed the main model in order to
	 * manipulate it as part of adding the transaction to the model. Model creates a
	 * clone of itself, clone is passed into transaction addition code that passes
	 * it to each entry, and returns the eventual chain of modificiations after each
	 * entry is through with it. The final copy passes out of the transaction code,
	 * back to the Model code, where the transaction is finally added, and then the
	 * final model copy is returned.
	 * 
	 * The process of adding the transaction can incrementally adjust accounts,
	 * categories, etc., because it is only when the process is complete that the
	 * user gets back a new model reference that has all the changes made. If there
	 * are problems before that, they get an exception instead, and the integrity of
	 * the model is preserved.
	 * 
	 * In this approach each entry is responsible for adding itself to the account
	 * or category or whatever it is associated with. These additions need to come
	 * with a way to link back to the transaction, so that editing from an account
	 * view can also allow changes to the associated category entry to maintain
	 * balance. So transaction entries have links to their parent transaction, and
	 * that linkage needs to remain consistent as the evolving set of immutable crap
	 * gets churned through. Transactions may need UUIDs behind the scenes to keep
	 * them identifiable.
	 * 
	 * Each account, cat., etc. then is able to find the xact for each of its
	 * entries, and the xact is able to handle modifications to its entries, and
	 * then the xact is able to run a set of updates on the model based on its
	 * entries to purge old entires and account for new ones. So removing an entry
	 * from a xact needs to be accounted for, but this can be done because the model
	 * knows the entries of the prior version of that xact UUID, and can remove them
	 * all, then process the new entries as provided by the updated xact.
	 * 
	 * When renaming an account, all xacts with entries that reference it need to be
	 * updated, but it is a name-only change, so does not need any remove/add cycle
	 * that recalculates balances. xacts and their entries need to support such
	 * behavior.
	 */

	Amount accountBalance(String accountName, boolean clearedOnly) throws ModelException {
		checkIdentifierType(accountName, IdentifierType.ACCOUNT);
		var account = accounts.get(accountName);
		if (account == null) {
			throw ModelException.accountNotExist(accountName);
		}
		assert (account.name().equals(accountName));
		return account.balance(clearedOnly);
	}

	List<String> accountNames(boolean onBudget) throws ModelException {
		var names = new ArrayList<String>();
		for (var accountEntry : accounts.entrySet()) {
			var account = accountEntry.getValue();
			assert (account.name().equals(accountEntry.getKey()));
			checkIdentifierType(account.name(), IdentifierType.ACCOUNT);
			if (account.onBudget() == onBudget) {
				names.add(account.name());
			}
		}
		return names;
	}

	void checkIdentifierType(String identifier, IdentifierType type) throws ModelException {
		var idType = identifiers.get(identifier);
		if (idType == null) {
			throw ModelException.unknownIdentifier(identifier);
		} else if (!idType.equals(type)) {
			throw ModelException.identifierMismatch(identifier, idType, type);
		}
	}

	IdentifierType getIdentifierType(String identifier) throws ModelException {
		var idType = identifiers.get(identifier);
		if (idType == null) {
			throw ModelException.unknownIdentifier(identifier);
		} else {
			return idType;
		}
	}

	Model withHiddenAccount(String name) throws ModelException {
		checkIdentifierType(name, IdentifierType.ACCOUNT);
		if (!accounts.containsKey(name)) {
			throw ModelException.accountNotExist(name);
		}
		if (accounts.get(name).hidden()) {
			throw ModelException.accountNotOpen(name);
		}

		var model = clone();
		model.accounts.put(name, model.accounts.get(name).asHidden());
		return model;
	}

	Model withManualBudgetPeriod(LocalDate startDate, Period duration) throws ModelException {
		var model = clone();

		// remove all automatic periods
		// TODO get the transactions from these periods to restore them later
		model.budgetPeriods.entrySet().removeIf(entry -> !entry.getValue().manual());

		// make the new period
		var period = new BudgetPeriod(startDate, duration).asManual();

		// make sure none of the remaining (manual) periods overlap
		for (var entry : model.budgetPeriods.entrySet()) {
			if (entry.getValue().dates().overlaps(period.dates())) {
				throw ModelException.budgetPeriodOverlap(period.dates(), entry.getValue().dates());
			}
		}

		// put the new period in place
		model.budgetPeriods.put(startDate, period);

		// regenerate automatic periods by filling in the gaps between manual periods
		NavigableMap<LocalDate, BudgetPeriod> autoPeriods = new TreeMap<>();
		for (var periodIt = model.budgetPeriods.entrySet().iterator(); periodIt.hasNext(); /* inside */) {
			var manualPeriod = periodIt.next().getValue();

			// if the next manual period does not exist, we are done iterating
			var nextEntry = model.budgetPeriods.ceilingEntry(manualPeriod.dates().startDate().plusDays(1));
			if (nextEntry == null) {
				break;
			}
			var nextPeriod = nextEntry.getValue();

			// create automatic periods based on the current manual period as long as their
			// start dates are prior to the next manual period start date
			for (var autoPeriod = manualPeriod.next(); autoPeriod.dates().startDate()
					.compareTo(nextPeriod.dates().startDate()) < 0; autoPeriod = autoPeriod.next()) {

				// but the automatic period may extend into the next manual period, so cut it
				// short to just fit if it does
				if (autoPeriod.dates().endDate().compareTo(nextPeriod.dates().startDate()) >= 0) {
					autoPeriod = autoPeriod.withEndDate(nextPeriod.dates().startDate().minusDays(1));
				}
				autoPeriods.put(autoPeriod.dates().startDate(), autoPeriod);
			}
		}

		// merge the auto periods in with the manual periods
		model.budgetPeriods.putAll(autoPeriods);

		// TODO assign the transactions that were part of the prior set of automatic
		// periods

		return model;
	}

	Model withOpenAccount(String name, boolean onBudget) throws ModelException {
		var exists = accounts.containsKey(name);
		if (exists) {
			if (!accounts.get(name).hidden()) {
				throw ModelException.accountAlreadyOpen(name);
			}
			if (accounts.get(name).onBudget() != onBudget) {
				throw ModelException.accountTypeMismatch(name);
			}
		}

		var model = clone();
		model.checkSetIdentifierType(name, IdentifierType.ACCOUNT);
		if (exists) {
			model.accounts.put(name, model.accounts.get(name).asUnhidden());
		} else {
			model.accounts.put(name, new Account(name, onBudget));
		}
		return model;
	}

	Model withoutTransaction(UUID id) throws ModelException {
		var xact = transactions.get(id);
		var model = xact.removeFromModel(this);
		model.transactions.remove(id);
		return model;
	}

	Model withoutTransactionEntry(AccountTransactionEntry entry) throws ModelException {
		checkIdentifierType(entry.account(), IdentifierType.ACCOUNT);
		var account = Optional.ofNullable(accounts.get(entry.account()))
				.orElseThrow(() -> ModelException.accountNotExist(entry.account()));
		var model = clone();
		model.accounts.put(account.name(), account.withoutEntry(entry));
		return model;
	}

	Model withTransaction(Transaction xact) throws ModelException {
		xact = xact.balanced();
		var xactOrig = transactions.get(xact.id());
		Model model;
		if (xactOrig != null) {
			model = withoutTransaction(xactOrig.id());
		} else {
			model = clone();
		}
		model = xact.addToModel(model);
		model.transactions.put(xact.id(), xact);
		return model;
	}

	Model withTransactionEntry(AccountTransactionEntry entry) throws ModelException {
		checkIdentifierType(entry.account(), IdentifierType.ACCOUNT);
		var account = Optional.ofNullable(accounts.get(entry.account()))
				.orElseThrow(() -> ModelException.accountNotExist(entry.account()));
		var model = clone();
		model.accounts.put(account.name(), account.withEntry(entry));
		return model;
	}

	// @Override
	protected Model clone() {
		var model = (Model) super.clone();
		model.accounts = new TreeMap<>(accounts);
		model.budgetPeriods = new TreeMap<>(budgetPeriods);
		model.identifiers = new TreeMap<>(identifiers);
		return model;
	}

	private void checkSetIdentifierType(String identifier, IdentifierType type) throws ModelException {
		var idType = identifiers.get(identifier);
		if (idType == null) {
			identifiers.put(identifier, type);
		} else if (!idType.equals(type)) {
			throw ModelException.identifierMismatch(identifier, idType, type);
		}
	}

}
