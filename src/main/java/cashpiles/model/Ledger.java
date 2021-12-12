package cashpiles.model;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.time.LocalDate;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.BlankLine;
import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.ledger.IncomeBudgetEntry;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.ManualGoalBudgetEntry;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.ReserveBudgetEntry;
import cashpiles.ledger.RoutineBudgetEntry;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.Transaction;
import cashpiles.ledger.UnbalancedTransaction;
import cashpiles.ledger.WithholdingBudgetEntry;
import cashpiles.time.DateRange;

// TODO what happens if the last category for a particular owner is closed? and what *should* happen?
public class Ledger implements ItemProcessor {

	private final Map<String, Account> accounts = new HashMap<>();
	private final TreeMap<String, Category> categories = new TreeMap<>();
	private final TreeMap<LocalDate, List<LedgerItem>> items = new TreeMap<>();
	private final List<ActionListener> listeners = new ArrayList<>();
	private final List<LedgerException> pendingExceptions = new ArrayList<>();
	private List<LedgerItem> preDatedItems = new ArrayList<>();

	public void add(AccountBalance balance) throws LedgerModelException {
		var account = accounts.get(balance.account());
		if (account == null) {
			throw LedgerModelException.forUnknown(balance);
		}
		account = account.reconciled(balance);
		accounts.put(balance.account(), account);
		insertEndOfDay(balance.date(), balance);
		notify("AccountBalance");
	}

	public void add(AccountCommand command) throws LedgerModelException {
		var account = accounts.get(command.account());
		switch (command.status()) {
		case ON_BUDGET, OFF_BUDGET -> {
			if (account != null) {
				if (account.status() != command.status()) {
					throw LedgerModelException.forTypeChange(command);
				}
				throw LedgerModelException.forAlreadyOpen(command);
			}

			// no validation exceptions past this point
			account = new Account(command.date(), command.status());
			accounts.put(command.account(), account);
		}
		case CLOSED -> {
			if (account == null) {
				throw LedgerModelException.forAlreadyClosed(command);
			}
			if (!account.balance().equals(new Amount())) {
				throw LedgerModelException.forNonZeroClose(command);
			}

			// no validation exceptions past this point
			accounts.remove(command.account());
		}
		}
		insertEndOfDay(command.date(), command);
		notify("AccountCommand");
	}

	public void add(BlankLine blank) {
		insertEnd(blank);
		notify("BlankLine");
	}

	public void add(Budget budget) throws LedgerException {
		// this gives us the budget again via the process methods, as well as its
		// entries, so we can validate the whole thing first
		budget.process(this);

		// if we got any exceptions (since they can't be thrown by a processor), throw
		// the first one here (the others wouldn't have made it anyway)
		if (!pendingExceptions.isEmpty()) {
			var firstEx = pendingExceptions.get(0);
			pendingExceptions.clear();
			throw firstEx;
		}

		// and then we can save it all at once safely
		insertEndOfDay(budget.date(), budget);
		notify("Transaction");
	}

	public void add(Transaction transaction) throws LedgerException {
		transaction.balance();

		// this gives us the transaction again via the process methods, as well as its
		// entries, so we can validate the whole thing first
		transaction.process(this);

		// if we got any exceptions (since they can't be thrown by a processor), throw
		// the first one here (the others wouldn't have made it anyway)
		if (!pendingExceptions.isEmpty()) {
			var firstEx = pendingExceptions.get(0);
			pendingExceptions.clear();
			throw firstEx;
		}

		// and then we can save it all at once safely
		insertEndOfDay(transaction.date(), transaction);
		notify("Transaction");
	}

	public void add(UnbalancedTransaction transaction) throws LedgerModelException {
		var account = accounts.get(transaction.account());
		if (account == null) {
			throw LedgerModelException.forUnknown(transaction);
		}
		if (account.status() != AccountCommand.Status.OFF_BUDGET) {
			throw LedgerModelException.forOffBudgetNeeded(transaction);
		}
		if (transaction.date().compareTo(account.startDate()) < 0) {
			throw LedgerModelException.forTooEarly(transaction, account.startDate());
		}

		// no exceptions past this point
		account = account.withTransaction(transaction);
		accounts.put(transaction.account(), account);
		insertEndOfDay(transaction.date(), transaction);
		notify("UnbalancedTransaction");
	}

	public void addListener(ActionListener listener) {
		listeners.add(listener);
	}

	public void process(ItemProcessor processor) {
		items.forEach((day, dayItems) -> {
			dayItems.forEach(item -> item.process(processor));
		});
		processor.finish();
	}

	private void insertEnd(LedgerItem item) {
		if (items.isEmpty()) {
			assert (preDatedItems != null);
			preDatedItems.add(item);
		} else {
			insertEndOfDay(items.lastKey(), item);
		}
	}

	private void insertEndOfDay(LocalDate date, LedgerItem item) {
		var dayItems = items.get(date);
		if (dayItems == null) {
			dayItems = new ArrayList<>();
			items.put(date, dayItems);
		}
		if (preDatedItems != null) {
			dayItems.addAll(preDatedItems);
			preDatedItems = null;
		}
		dayItems.add(item);
	}

	private void notify(String eventName) {
		for (var listener : listeners) {
			listener.actionPerformed(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, eventName));
		}
	}

	@Override
	public void process(AccountTransactionEntry entry) {
		var account = accounts.get(entry.account());
		if (account == null) {
			pendingExceptions.add(LedgerModelException.forUnknown(entry));
			return;
		}
		if (account.status() != AccountCommand.Status.ON_BUDGET) {
			pendingExceptions.add(LedgerModelException.forBudgetNeeded(entry));
			return;
		}
		if (entry.parent().date().compareTo(account.startDate()) < 0) {
			pendingExceptions.add(LedgerModelException.forTooEarly(entry, account.startDate()));
			return;
		}

		// TODO this is still wrong for exception safety, since we are modifying the
		// internal state when a following entry may throw an exception
		// no exceptions past this point
		account = account.withTransaction(entry);
		accounts.put(entry.account(), account);
	}

	@Override
	public boolean process(Budget budget) {
		try {
			for (var entry : categories.entrySet()) {
				entry.setValue(entry.getValue().withPeriods(budget));
			}
			return true;
		} catch (LedgerModelException ex) {
			pendingExceptions.add(ex);
			return false;
		}
	}

	@Override
	public void process(CategoryTransactionEntry entry) {
		var category = categories.get(entry.category());
		if (category == null) {
			pendingExceptions.add(LedgerModelException.forUnknown(entry));
			return;
		}

		try {
			for (var c : categories.entrySet()) {
				c.setValue(c.getValue().withDate(entry));
			}
			category = category.withTransaction(entry);
			categories.put(entry.category(), category);
		} catch (LedgerModelException ex) {
			pendingExceptions.add(ex);
			return;
		}
		entry.trackingAccount().ifPresent(trackingAccount -> processTracking(entry, trackingAccount));
	}

	@Override
	public void process(GoalBudgetEntry entry) {
		var category = categories.get(entry.name());
		if (category != null) {
			pendingExceptions.add(LedgerModelException.forExistingCategory(entry));
			return;
		}
		category = new GoalCategory(entry.parent().date(),
				new DateRange(entry.parent().date(), entry.parent().period()));
		categories.put(entry.name(), category);
	}

	@Override
	public void process(IncomeBudgetEntry entry) {
		var category = categories.get(entry.name());
		if (category != null) {
			pendingExceptions.add(LedgerModelException.forExistingCategory(entry));
			return;
		}
		category = new IncomeCategory(entry.parent().date(),
				new DateRange(entry.parent().date(), entry.parent().period()));
		categories.put(entry.name(), category);
	}

	@Override
	public void process(ManualGoalBudgetEntry entry) {
		var category = categories.get(entry.name());
		if (category != null) {
			pendingExceptions.add(LedgerModelException.forExistingCategory(entry));
			return;
		}
		category = new ManualGoalCategory(entry.parent().date(),
				new DateRange(entry.parent().date(), entry.parent().period()));
		categories.put(entry.name(), category);
	}

	@Override
	public void process(ReserveBudgetEntry entry) {
		var category = categories.get(entry.name());
		if (category != null) {
			pendingExceptions.add(LedgerModelException.forExistingCategory(entry));
			return;
		}
		category = new ReserveCategory(entry.parent().date(),
				new DateRange(entry.parent().date(), entry.parent().period()));
		categories.put(entry.name(), category);
	}

	@Override
	public void process(RoutineBudgetEntry entry) {
		var category = categories.get(entry.name());
		if (category != null) {
			pendingExceptions.add(LedgerModelException.forExistingCategory(entry));
			return;
		}
		category = new RoutineCategory(entry.parent().date(),
				new DateRange(entry.parent().date(), entry.parent().period()));
		categories.put(entry.name(), category);
	}

	@Override
	public void process(OwnerTransactionEntry entry) {
		entry.trackingAccount().ifPresent(trackingAccount -> processTracking(entry, trackingAccount));
	}

	@Override
	public boolean process(Transaction transaction) {
		return true;
	}

	@Override
	public void process(WithholdingBudgetEntry entry) {
		var category = categories.get(entry.name());
		if (category != null) {
			pendingExceptions.add(LedgerModelException.forExistingCategory(entry));
			return;
		}
		category = new WithholdingCategory(entry.parent().date(),
				new DateRange(entry.parent().date(), entry.parent().period()));
		categories.put(entry.name(), category);
	}

	private void processTracking(TrackingTransactionEntry entry, String accountName) {
		var account = accounts.get(accountName);
		if (account == null) {
			pendingExceptions.add(LedgerModelException.forUnknown(entry));
			return;
		}
		if (account.status() != AccountCommand.Status.OFF_BUDGET) {
			pendingExceptions.add(LedgerModelException.forOffBudgetNeeded(entry));
			return;
		}
		if (entry.parent().date().compareTo(account.startDate()) < 0) {
			pendingExceptions.add(LedgerModelException.forTooEarly(entry, account.startDate()));
			return;
		}

		// TODO this is still wrong for exception safety, since we are modifying the
		// internal state when a following entry may throw an exception
		// no exceptions past this point
		account = account.withTransaction(entry);
		accounts.put(accountName, account);
	}

}
