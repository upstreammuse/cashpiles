package cashpiles.budget;

import java.time.LocalDate;

import cashpiles.time.DateRange;

@SuppressWarnings("serial")
public class BudgetReconfigureException extends Exception {

	public BudgetReconfigureException(DateRange dateRange) {
		super("Cannot reconfigure budget to dates " + dateRange + " because some dates would no longer be included");
	}

	public BudgetReconfigureException(DateRange dates, LocalDate date) {
		super("Can only reconfigure budget with dates " + dates + " on first date, not " + date);
	}

}
