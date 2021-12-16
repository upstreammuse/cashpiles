package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class IncomeCategory extends Category {

	IncomeCategory(LocalDate startDate, DateRange startPeriod, String owner) {
		super(startDate, startPeriod, owner);
	}

	@Override
	public IncomeCategory clone() {
		return (IncomeCategory) super.clone();
	}

	@Override
	public String type() {
		return "Income";
	}

}
