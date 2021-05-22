package cashpiles.budget;

import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.time.DateRange;

@SuppressWarnings("serial")
public class BudgetException extends LedgerException {

	public static BudgetException forCategoryClosure(CloseBudgetEntry entry) {
		return new BudgetException(entry,
				"Cannot close budget category '" + entry.category + "' that already has activity this period");
	}

	public static BudgetException forClosedCategory(CloseBudgetEntry entry) {
		return new BudgetException(entry, "Cannot close budget category '" + entry.category + "' that isn't open");
	}

	public static BudgetException forDateRange(Budget budget) {
		var dateRange = new DateRange(budget.date, budget.period);
		return new BudgetException(budget,
				"Cannot reconfigure budget to dates " + dateRange + " because some dates would no longer be included");
	}

	public static BudgetException forReconfigure(Budget budget, DateRange dates) {
		return new BudgetException(budget,
				"Can only reconfigure budget with dates " + dates + " on first date, not " + budget.date);
	}

	public static BudgetException forUnknownCategory(CategoryTransactionEntry entry) {
		return new BudgetException(entry, "Cannot use unknown budget category '" + entry.category + "'");
	}

	public static BudgetException forUnknownOwner(OwnerTransactionEntry entry) {
		return new BudgetException(entry, "Cannot use unknown budget category owner '" + entry.owner + "'");
	}

	private BudgetException(LedgerItem item, String message) {
		super(item, message);
	}

}
