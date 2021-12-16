package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class GoalCategory extends Category {

	GoalCategory(LocalDate startDate, DateRange startPeriod, String owner) {
		super(startDate, startPeriod, owner);
	}

	@Override
	public GoalCategory clone() {
		return (GoalCategory) super.clone();
	}

	@Override
	public String type() {
		return "Goal";
	}

}
