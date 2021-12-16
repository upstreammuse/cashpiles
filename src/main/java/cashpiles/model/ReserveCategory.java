package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class ReserveCategory extends Category {

	ReserveCategory(LocalDate startDate, DateRange startPeriod, String owner) {
		super(startDate, startPeriod, owner);
	}

	@Override
	public ReserveCategory clone() {
		return (ReserveCategory) super.clone();
	}

	@Override
	public String type() {
		return "Reserve";
	}

}
