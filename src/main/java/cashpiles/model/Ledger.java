package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.BlankLine;
import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.DatedLedgerItem;
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
import cashpiles.ledger.TransactionException;
import cashpiles.ledger.UnbalancedTransaction;
import cashpiles.ledger.WithholdingBudgetEntry;
import cashpiles.model2.AccountsView;
import cashpiles.model2.StatementsView;

// TODO what happens if the last category for a particular owner is closed? and what *should* happen?
public class Ledger implements ItemProcessor {

	private final AccountsMap accounts = new AccountsMap();
	private final Set<String> categories = new TreeSet<>();
	private final TreeMap<LocalDate, List<LedgerItem>> items = new TreeMap<>();
	private Optional<List<LedgerItem>> preDatedItems = Optional.of(new ArrayList<>());

	public AccountsView getAccounts() {
		return accounts;
	}

	public StatementsView getStatements(String accountName) {
		return accounts.get(accountName);
	}

	private void insertEnd(LedgerItem item) {
		preDatedItems.ifPresentOrElse(items -> items.add(item), () -> insertEnd(items.lastKey(), item));
	}

	private void insertEnd(DatedLedgerItem item) {
		insertEnd(item.date(), item);
	}

	private void insertEnd(LocalDate date, LedgerItem item) {
		var dayItems = items.getOrDefault(date, new ArrayList<>());
		preDatedItems.ifPresent(items -> {
			dayItems.addAll(items);
			preDatedItems = Optional.empty();
		});
		dayItems.add(item);
		items.put(date, dayItems);
	}

	@Override
	public void process(AccountBalance balance) throws LedgerModelException {
		var account = accounts.get(balance.account());
		if (account == null) {
			throw LedgerModelException.forUnknown(balance);
		}
		account = account.reconciled(balance);
		accounts.put(balance.account(), account);
		insertEnd(balance);
	}

	@Override
	public void process(AccountCommand command) throws LedgerModelException {
		var account = accounts.get(command.account());
		switch (command.status()) {
		case ON_BUDGET, OFF_BUDGET -> {
			if (account != null) {
				if (account.status() != command.status()) {
					throw LedgerModelException.forTypeChange(command);
				}
				throw LedgerModelException.forAlreadyOpen(command);
			}
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
			accounts.remove(command.account());
		}
		}
		insertEnd(command);
	}

	@Override
	public void process(AccountTransactionEntry entry) throws LedgerModelException {
		var account = accounts.get(entry.account());
		if (account == null) {
			throw LedgerModelException.forUnknown(entry);
		}
		if (account.status() != AccountCommand.Status.ON_BUDGET) {
			throw LedgerModelException.forBudgetNeeded(entry);
		}
		if (entry.parent().date().compareTo(account.startDate()) < 0) {
			throw LedgerModelException.forTooEarly(entry, account.startDate());
		}
		account = account.withTransaction(entry);
		accounts.put(entry.account(), account);
	}

	@Override
	public void process(BlankLine blank) {
		insertEnd(blank);
	}

	@Override
	public boolean process(Budget budget) throws LedgerModelException {
		insertEnd(budget);
		return true;
	}

	@Override
	public void process(CategoryTransactionEntry entry) throws LedgerModelException {
		if (!categories.contains(entry.category())) {
			throw LedgerModelException.forUnknown(entry);
		}
		processTracking(entry);
	}

	@Override
	public void process(GoalBudgetEntry entry) throws LedgerModelException {
		if (categories.contains(entry.name())) {
			throw LedgerModelException.forExistingCategory(entry);
		}
		categories.add(entry.name());
	}

	@Override
	public void process(IncomeBudgetEntry entry) throws LedgerModelException {
		if (categories.contains(entry.name())) {
			throw LedgerModelException.forExistingCategory(entry);
		}
		categories.add(entry.name());
	}

	public void process(ItemProcessor processor) throws LedgerException {
		for (var entry : items.entrySet()) {
			for (var dayItem : entry.getValue()) {
				dayItem.process(processor);
			}
		}
		processor.finish();
	}

	@Override
	public void process(ManualGoalBudgetEntry entry) throws LedgerModelException {
		if (categories.contains(entry.name())) {
			throw LedgerModelException.forExistingCategory(entry);
		}
		categories.add(entry.name());
	}

	@Override
	public void process(ReserveBudgetEntry entry) throws LedgerModelException {
		if (categories.contains(entry.name())) {
			throw LedgerModelException.forExistingCategory(entry);
		}
		categories.add(entry.name());
	}

	@Override
	public void process(RoutineBudgetEntry entry) throws LedgerModelException {
		if (categories.contains(entry.name())) {
			throw LedgerModelException.forExistingCategory(entry);
		}
		categories.add(entry.name());
	}

	@Override
	public void process(OwnerTransactionEntry entry) throws LedgerModelException {
		processTracking(entry);
	}

	@Override
	public boolean process(Transaction transaction) throws TransactionException {
		insertEnd(transaction.withBalance());
		return true;
	}

	@Override
	public void process(UnbalancedTransaction transaction) throws LedgerModelException {
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
		account = account.withTransaction(transaction);
		accounts.put(transaction.account(), account);
		insertEnd(transaction);
	}

	@Override
	public void process(WithholdingBudgetEntry entry) throws LedgerModelException {
		if (categories.contains(entry.name())) {
			throw LedgerModelException.forExistingCategory(entry);
		}
		categories.add(entry.name());
	}

	private void processTracking(TrackingTransactionEntry entry) throws LedgerModelException {
		var accountName = entry.trackingAccount().orElse("");
		if (accountName.isEmpty()) {
			return;
		}
		var account = accounts.get(accountName);
		if (account == null) {
			throw LedgerModelException.forUnknown(entry);
		}
		if (account.status() != AccountCommand.Status.OFF_BUDGET) {
			throw LedgerModelException.forOffBudgetNeeded(entry);
		}
		if (entry.parent().date().compareTo(account.startDate()) < 0) {
			throw LedgerModelException.forTooEarly(entry, account.startDate());
		}
		account = account.withTransaction(entry);
		accounts.put(accountName, account);
	}

}
