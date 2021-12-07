package cashpiles.budget;

import cashpiles.ledger.Budget;
import cashpiles.ledger.BudgetEntry;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.time.DateRange;

@SuppressWarnings("serial")
public class BudgetException extends LedgerException {

	static BudgetException forCategoryClosure(CloseBudgetEntry entry) {
		return new BudgetException(entry,
				"Cannot close budget category '" + entry.name() + "' that already has activity this period");
	}

	static BudgetException forClosedCategory(CloseBudgetEntry entry) {
		return new BudgetException(entry, "Cannot close budget category '" + entry.name() + "' that isn't open");
	}

	static BudgetException forDateRange(Budget budget) {
		var dateRange = new DateRange(budget.date(), budget.period());
		return new BudgetException(budget,
				"Cannot reconfigure budget to dates " + dateRange + " because some dates would no longer be included");
	}

	static BudgetException forExistingCategory(BudgetEntry entry) {
		return new BudgetException(entry, "Cannot create budget category '" + entry.name() + "' that already exists");
	}

	// TODO this may indicate that this exception and the thing using this method
	// should be in the same package
	public static BudgetException forReconfigure(Budget budget, DateRange dates) {
		return new BudgetException(budget,
				"Can only reconfigure budget with dates " + dates + " on first date, not " + budget.date());
	}

	// TODO this may indicate that this exception and the thing using this method
	// should be in the same package
	public static BudgetException forTransactionDate(TrackingTransactionEntry entry, DateRange dates) {
		return new BudgetException(entry, "Cannot add transaction dated " + entry.parent().date()
				+ " to current budget period with dates " + dates.startDate() + " to " + dates.endDate());
	}

	static BudgetException forUnknownCategory(CategoryTransactionEntry entry) {
		return new BudgetException(entry, "Cannot use unknown budget category '" + entry.category() + "'");
	}

	static BudgetException forUnknownOwner(OwnerTransactionEntry entry) {
		return new BudgetException(entry, "Cannot use unknown budget category owner '" + entry.owner() + "'");
	}

	protected BudgetException(LedgerItem item, String message) {
		super(item, message);
	}

}
