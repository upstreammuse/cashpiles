package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class RoutineCategory extends Category {

	RoutineCategory(LocalDate startDate, DateRange startPeriod) {
		super(startDate, startPeriod);
	}

	@Override
	public RoutineCategory clone() {
		return (RoutineCategory) super.clone();
	}

}
