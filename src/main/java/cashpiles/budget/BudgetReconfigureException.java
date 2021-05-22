package cashpiles.budget;

import java.time.LocalDate;

import cashpiles.time.DateRange;

@SuppressWarnings("serial")
public class BudgetReconfigureException extends Exception {

	public static BudgetReconfigureException forCategoryClosure(String name) {
		return new BudgetReconfigureException(
				"Cannot close budget category '" + name + "' that already has activity this period");
	}

	public static BudgetReconfigureException forClosedCategory(String name) {
		return new BudgetReconfigureException("Cannot close budget category '" + name + "' that isn't open");
	}

	public static BudgetReconfigureException forDateRange(DateRange dateRange) {
		return new BudgetReconfigureException(
				"Cannot reconfigure budget to dates " + dateRange + " because some dates would no longer be included");
	}

	public static BudgetReconfigureException forReconfigure(DateRange dates, LocalDate date) {
		return new BudgetReconfigureException(
				"Can only reconfigure budget with dates " + dates + " on first date, not " + date);
	}

	private BudgetReconfigureException(String message) {
		super(message);
	}

}
