package cashpiles.budget;

import java.time.LocalDate;

import cashpiles.time.DateRange;

@SuppressWarnings("serial")
public class BudgetReconfigureException extends Exception {

	static BudgetReconfigureException forCategoryClosure(String name) {
		return new BudgetReconfigureException(
				"Cannot close budget category '" + name + "' that already has activity this period");
	}

	static BudgetReconfigureException forClosedCategory(String name) {
		return new BudgetReconfigureException("Cannot close budget category '" + name + "' that isn't open");
	}

	// TODO use static like above
	public BudgetReconfigureException(DateRange dateRange) {
		super("Cannot reconfigure budget to dates " + dateRange + " because some dates would no longer be included");
	}

	// TODO use static like above
	public BudgetReconfigureException(DateRange dates, LocalDate date) {
		super("Can only reconfigure budget with dates " + dates + " on first date, not " + date);
	}

	private BudgetReconfigureException(String message) {
		super(message);
	}
}
