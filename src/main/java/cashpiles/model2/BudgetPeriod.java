package cashpiles.model2;

import java.time.LocalDate;
import java.time.Period;

import cashpiles.time.DateRange;

class BudgetPeriod extends ModelBase {

	private DateRange dates;
	private boolean manual = false;

	BudgetPeriod(LocalDate startDate, Period duration) {
		dates = new DateRange(startDate, duration);
	}

	BudgetPeriod asManual() {
		var period = clone();
		period.manual = true;
		return period;
	}

	DateRange dates() {
		return dates;
	}

	boolean manual() {
		return manual;
	}

	BudgetPeriod next() {
		var entry = clone();
		entry.dates = entry.dates.next();
		entry.manual = false;
		return entry;
	}

	BudgetPeriod withEndDate(LocalDate endDate) {
		var period = clone();
		period.dates = new DateRange(period.dates.startDate(), endDate);
		return period;
	}

	// @Override
	protected BudgetPeriod clone() {
		return (BudgetPeriod) super.clone();
	}

}
