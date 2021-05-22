package cashpiles.budget;

import cashpiles.ledger.Budget;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.time.DateRange;

@SuppressWarnings("serial")
public class BudgetReconfigureException extends LedgerException {

	public static BudgetReconfigureException forCategoryClosure(CloseBudgetEntry entry) {
		return new BudgetReconfigureException(entry,
				"Cannot close budget category '" + entry.category + "' that already has activity this period");
	}

	public static BudgetReconfigureException forClosedCategory(CloseBudgetEntry entry) {
		return new BudgetReconfigureException(entry,
				"Cannot close budget category '" + entry.category + "' that isn't open");
	}

	public static BudgetReconfigureException forDateRange(Budget budget) {
		var dateRange = new DateRange(budget.date, budget.period);
		return new BudgetReconfigureException(budget,
				"Cannot reconfigure budget to dates " + dateRange + " because some dates would no longer be included");
	}

	public static BudgetReconfigureException forReconfigure(Budget budget, DateRange dates) {
		return new BudgetReconfigureException(budget,
				"Can only reconfigure budget with dates " + dates + " on first date, not " + budget.date);
	}

	private BudgetReconfigureException(LedgerItem item, String message) {
		super(item, message);
	}

}
