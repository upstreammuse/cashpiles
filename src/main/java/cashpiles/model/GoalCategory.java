package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class GoalCategory extends Category {

	GoalCategory(LocalDate startDate, DateRange startPeriod) {
		super(startDate, startPeriod);
	}

	@Override
	public GoalCategory clone() {
		return (GoalCategory) super.clone();
	}

}
