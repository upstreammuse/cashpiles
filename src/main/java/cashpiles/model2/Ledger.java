package cashpiles.model2;

import java.util.ArrayList;
import java.util.List;

import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.DatedLedgerItem;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.Transaction;
import cashpiles.util.Lists;

// this is *not* a data class, and mutates as data is fed to it for processing
class Ledger implements ItemProcessor {

	private class BudgetEntryProcessor implements ItemProcessor {

		@Override
		public void process(CloseBudgetEntry entry) throws ModelException {
			ensurePeriod(entry.parent());
			var period = Lists.lastOf(periods);
			periods.remove(period);
			periods.add(period.close(entry));
		}

		@Override
		public void process(GoalBudgetEntry entry) throws ModelException {
			ensurePeriod(entry.parent());
			var period = Lists.lastOf(periods);
			periods.remove(period);
			periods.add(period.withCategory(entry));
		}

	}

	private class NullProcessor implements ItemProcessor {

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

	}

	private class TransactionEntryProcessor implements ItemProcessor {

		@Override
		public void process(CategoryTransactionEntry entry) throws ModelException {
			ensurePeriod(entry.parent());
			var period = Lists.lastOf(periods);
			periods.remove(period);
			periods.add(period.withTransaction(entry));
		}

	}

	private ItemProcessor budgetEntryProcessor = new NullProcessor();
	private List<BudgetPeriod> periods = new ArrayList<>();
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
		if (!Lists.lastOf(periods).dates().contains(item.date())) {
			throw ModelException.forOutOfRange(item);
		}
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
			if (!Lists.lastOf(periods).dates().next().startDate().equals(budget.date())) {
				throw ModelException.forOutOfSync(budget);
			} else {
				periods.add(Lists.lastOf(periods).next(budget));
			}
		}
		budgetEntryProcessor = new BudgetEntryProcessor();
		transactionEntryProcessor = new NullProcessor();
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

	@Override
	public boolean process(Transaction transaction) throws ModelException {
		ensurePeriod(transaction);
		budgetEntryProcessor = new NullProcessor();
		transactionEntryProcessor = new TransactionEntryProcessor();
		return true;
	}

}
