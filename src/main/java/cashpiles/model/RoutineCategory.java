package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class RoutineCategory extends Category {

	RoutineCategory(LocalDate startDate, DateRange startPeriod, String owner) {
		super(startDate, startPeriod, owner);
	}

	@Override
	public RoutineCategory clone() {
		return (RoutineCategory) super.clone();
	}

	@Override
	public String type() {
		return "Routine";
	}

}
