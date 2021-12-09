package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class ReserveCategory extends Category {

	ReserveCategory(LocalDate startDate, DateRange startPeriod) {
		super(startDate, startPeriod);
	}

	@Override
	public ReserveCategory clone() {
		return (ReserveCategory) super.clone();
	}

}
