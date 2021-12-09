package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class ManualGoalCategory extends Category {

	ManualGoalCategory(LocalDate startDate, DateRange startPeriod) {
		super(startDate, startPeriod);
	}

	@Override
	public ManualGoalCategory clone() {
		return (ManualGoalCategory) super.clone();
	}

}
