package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class WithholdingCategory extends Category {

	WithholdingCategory(LocalDate startDate, DateRange startPeriod) {
		super(startDate, startPeriod);
	}

	@Override
	public WithholdingCategory clone() {
		return (WithholdingCategory) super.clone();
	}

}
