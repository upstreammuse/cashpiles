package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.TreeMap;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.BlankLine;
import cashpiles.ledger.Budget;
import cashpiles.ledger.BudgetEntry;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
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
import cashpiles.ledger.TransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;
import cashpiles.ledger.WithholdingBudgetEntry;
import cashpiles.util.Lists;

// this is *not* a data class, and mutates as data is fed to it for processing
public class Ledger implements ItemProcessor {

	private class BudgetEntryProcessor implements ItemProcessor {

		private BudgetPeriod common(BudgetEntry entry) throws ModelException {
			ensurePeriod(entry.parent());
			var period = Lists.lastOf(periods);
			periods.remove(period);
			return period;
		}

		@Override
		public void process(CloseBudgetEntry entry) throws ModelException {
			var period = common(entry);
			periods.add(period.close(entry));
		}

		@Override
		public void process(GoalBudgetEntry entry) throws ModelException {
			var period = common(entry);
			periods.add(period.withCategory(entry));
		}

		@Override
		public void process(IncomeBudgetEntry entry) throws ModelException {
			var period = common(entry);
			periods.add(period.withCategory(entry));
		}

		@Override
		public void process(ManualGoalBudgetEntry entry) throws ModelException {
			var period = common(entry);
			periods.add(period.withCategory(entry));
		}

		@Override
		public void process(ReserveBudgetEntry entry) throws ModelException {
			var period = common(entry);
			periods.add(period.withCategory(entry));
		}

		@Override
		public void process(RoutineBudgetEntry entry) throws ModelException {
			var period = common(entry);
			periods.add(period.withCategory(entry));
		}

		@Override
		public void process(WithholdingBudgetEntry entry) throws ModelException {
			var period = common(entry);
			periods.add(period.withCategory(entry));
		}

	}

	private static class NullProcessor implements ItemProcessor {

		@Override
		public void process(AccountTransactionEntry entry) throws ModelException {
			throw ModelException.forOrphan(entry);
		}

		@Override
		public void process(CategoryTransactionEntry entry) throws ModelException {
			throw ModelException.forOrphan(entry);
		}

		@Override
		public void process(CloseBudgetEntry entry) throws ModelException {
			throw ModelException.forOrphan(entry);
		}

		@Override
		public void process(GoalBudgetEntry entry) throws ModelException {
			throw ModelException.forOrphan(entry);
		}

		@Override
		public void process(IncomeBudgetEntry entry) throws ModelException {
			throw ModelException.forOrphan(entry);
		}

		@Override
		public void process(ManualGoalBudgetEntry entry) throws ModelException {
			throw ModelException.forOrphan(entry);
		}

		@Override
		public void process(ReserveBudgetEntry entry) throws ModelException {
			throw ModelException.forOrphan(entry);
		}

		@Override
		public void process(OwnerTransactionEntry entry) throws ModelException {
			throw ModelException.forOrphan(entry);
		}

		@Override
		public void process(RoutineBudgetEntry entry) throws ModelException {
			throw ModelException.forOrphan(entry);
		}

		@Override
		public void process(WithholdingBudgetEntry entry) throws ModelException {
			throw ModelException.forOrphan(entry);
		}

	}

	private class TransactionEntryProcessor implements ItemProcessor {

		private BudgetPeriod common(TransactionEntry entry) throws ModelException {
			ensurePeriod(entry.parent());
			var period = Lists.lastOf(periods);
			periods.remove(period);
			return period;
		}

		@Override
		public void process(AccountTransactionEntry entry) throws ModelException {
			var account = accounts.get(entry.account());
			if (account == null) {
				throw ModelException.forUnknown(entry);
			}
			if (account.status() != AccountCommand.Status.ON_BUDGET) {
				throw ModelException.forBudgetNeeded(entry);
			}
			if (entry.parent().date().compareTo(account.startDate()) < 0) {
				throw ModelException.forTooEarly(entry, account.startDate());
			}
			account = account.withTransaction(entry);
			accounts.put(entry.account(), account);
		}

		@Override
		public void process(CategoryTransactionEntry entry) throws ModelException {
			processTracking(entry);
			var period = common(entry);
			periods.add(period.withTransaction(entry));
		}

		@Override
		public void process(OwnerTransactionEntry entry) throws ModelException {
			processTracking(entry);
			var period = common(entry);
			periods.add(period.withTransaction(entry));
		}

		private void processTracking(TrackingTransactionEntry entry) throws ModelException {
			var accountName = entry.trackingAccount().orElse("");
			if (accountName.isEmpty()) {
				return;
			}
			var account = accounts.get(accountName);
			if (account == null) {
				throw ModelException.forUnknown(entry);
			}
			if (account.status() != AccountCommand.Status.OFF_BUDGET) {
				throw ModelException.forOffBudgetNeeded(entry);
			}
			if (entry.parent().date().compareTo(account.startDate()) < 0) {
				throw ModelException.forTooEarly(entry, account.startDate());
			}
			account = account.withTransaction(entry);
			accounts.put(accountName, account);
		}

	}

	private final AccountsMap accounts = new AccountsMap();
	private ItemProcessor budgetEntryProcessor = new NullProcessor();
	private final TreeMap<LocalDate, List<LedgerItem>> items = new TreeMap<>();
	private final PeriodsList periods = new PeriodsList();
	private Optional<List<LedgerItem>> preDatedItems = Optional.of(new ArrayList<>());
	private ItemProcessor transactionEntryProcessor = new NullProcessor();

	// make sure there's at least one budget period, and that the budget covers the
	// latest date
	private void ensurePeriod(DatedLedgerItem item) throws ModelException {
		if (periods.isEmpty()) {
			throw ModelException.forNoBudget(item);
		}
		while (Lists.lastOf(periods).dates().endDate().compareTo(item.date()) < 0) {
			periods.add(Lists.lastOf(periods).next());
		}
		var lastDates = Lists.lastOf(periods).dates();
		if (!lastDates.contains(item.date())) {
			throw ModelException.forOutOfRange(item, lastDates);
		}
	}

	public AccountsView getAccounts() {
		return accounts;
	}

	public PeriodsView getPeriods() {
		return periods;
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
	public void process(AccountBalance balance) throws ModelException {
		var account = accounts.get(balance.account());
		if (account == null) {
			throw ModelException.forUnknown(balance);
		}
		account = account.reconciled(balance);
		accounts.put(balance.account(), account);
		budgetEntryProcessor = new NullProcessor();
		transactionEntryProcessor = new NullProcessor();
		insertEnd(balance);
	}

	@Override
	public void process(AccountCommand command) throws ModelException {
		var account = accounts.get(command.account());
		switch (command.status()) {
		case ON_BUDGET, OFF_BUDGET -> {
			if (account != null) {
				if (account.status() != command.status()) {
					throw ModelException.forTypeChange(command);
				}
				throw ModelException.forAlreadyOpen(command);
			}
			account = new Account(command.date(), command.status());
			accounts.put(command.account(), account);
		}
		case CLOSED -> {
			if (account == null) {
				throw ModelException.forAlreadyClosed(command);
			}
			if (!account.balance().equals(new Amount())) {
				throw ModelException.forNonZeroClose(command);
			}
			accounts.remove(command.account());
		}
		}
		budgetEntryProcessor = new NullProcessor();
		transactionEntryProcessor = new NullProcessor();
		insertEnd(command);
	}

	@Override
	public void process(AccountTransactionEntry entry) throws LedgerException {
		// if a transaction is active, this will be processed, and if not, this will
		// throw an exception
		transactionEntryProcessor.process(entry);
	}

	@Override
	public void process(BlankLine blank) {
		budgetEntryProcessor = new NullProcessor();
		transactionEntryProcessor = new NullProcessor();
		insertEnd(blank);
	}

	@Override
	public boolean process(Budget budget) throws ModelException {
		// if no periods yet, the provided budget entry can be the first one as-is
		if (periods.isEmpty()) {
			periods.add(new BudgetPeriod(budget));
		} else {
			// as long as the next period would start *before* the provided budget entry,
			// generate it
			while (Lists.lastOf(periods).dates().next().startDate().compareTo(budget.date()) < 0) {
				periods.add(Lists.lastOf(periods).next());
			}
			// at this point, either the next period starts on the date of the provided
			// budget entry, which is allowed, or the next period starts after the provided
			// budget entry, which is an error meaning the provided entry doesn't match the
			// existing cadence of budget periods
			var nextDate = Lists.lastOf(periods).dates().next().startDate();
			if (!nextDate.equals(budget.date())) {
				throw ModelException.forBudgetStartDate(budget, nextDate);
			} else {
				periods.add(Lists.lastOf(periods).next(budget));
			}
		}
		budgetEntryProcessor = new BudgetEntryProcessor();
		transactionEntryProcessor = new NullProcessor();
		insertEnd(budget);
		return true;
	}

	@Override
	public void process(CategoryTransactionEntry entry) throws LedgerException {
		// if a transaction is active, this will be processed, and if not, this will
		// throw an exception
		transactionEntryProcessor.process(entry);
	}

	@Override
	public void process(CloseBudgetEntry entry) throws LedgerException {
		// if a budget command is active, this will be processed, and if not, this will
		// throw an exception
		budgetEntryProcessor.process(entry);
	}

	@Override
	public void process(GoalBudgetEntry entry) throws LedgerException {
		// if a budget command is active, this will be processed, and if not, this will
		// throw an exception
		budgetEntryProcessor.process(entry);
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
	public void process(IncomeBudgetEntry entry) throws LedgerException {
		// if a budget command is active, this will be processed, and if not, this will
		// throw an exception
		budgetEntryProcessor.process(entry);
	}

	@Override
	public void process(ManualGoalBudgetEntry entry) throws LedgerException {
		// if a budget command is active, this will be processed, and if not, this will
		// throw an exception
		budgetEntryProcessor.process(entry);
	}

	@Override
	public void process(OwnerTransactionEntry entry) throws LedgerException {
		// if a transaction is active, this will be processed, and if not, this will
		// throw an exception
		transactionEntryProcessor.process(entry);
	}

	@Override
	public void process(ReserveBudgetEntry entry) throws LedgerException {
		// if a budget command is active, this will be processed, and if not, this will
		// throw an exception
		budgetEntryProcessor.process(entry);
	}

	@Override
	public void process(RoutineBudgetEntry entry) throws LedgerException {
		// if a budget command is active, this will be processed, and if not, this will
		// throw an exception
		budgetEntryProcessor.process(entry);
	}

	@Override
	public boolean process(Transaction transaction) throws ModelException {
		ensurePeriod(transaction);
		budgetEntryProcessor = new NullProcessor();
		transactionEntryProcessor = new TransactionEntryProcessor();
		insertEnd(transaction);
		return true;
	}

	@Override
	public void process(UnbalancedTransaction transaction) throws ModelException {
		var account = accounts.get(transaction.account());
		if (account == null) {
			throw ModelException.forUnknown(transaction);
		}
		if (account.status() != AccountCommand.Status.OFF_BUDGET) {
			throw ModelException.forOffBudgetNeeded(transaction);
		}
		if (transaction.date().compareTo(account.startDate()) < 0) {
			throw ModelException.forTooEarly(transaction, account.startDate());
		}
		account = account.withTransaction(transaction);
		accounts.put(transaction.account(), account);
		budgetEntryProcessor = new NullProcessor();
		transactionEntryProcessor = new NullProcessor();
		insertEnd(transaction);
	}

	@Override
	public void process(WithholdingBudgetEntry entry) throws LedgerException {
		// if a budget command is active, this will be processed, and if not, this will
		// throw an exception
		budgetEntryProcessor.process(entry);
	}

}
