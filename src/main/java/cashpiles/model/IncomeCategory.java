package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class IncomeCategory extends Category {

	IncomeCategory(LocalDate startDate, DateRange startPeriod) {
		super(startDate, startPeriod);
	}

	@Override
	public IncomeCategory clone() {
		return (IncomeCategory) super.clone();
	}

}
