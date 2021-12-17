package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class ManualGoalCategory extends Category {

	ManualGoalCategory(LocalDate startDate, DateRange startPeriod, String owner) {
		super(startDate, startPeriod, owner);
	}

	@Override
	BudgetPeriod allocate(BudgetPeriod period) {
		return period;
	}

	@Override
	public ManualGoalCategory clone() {
		return (ManualGoalCategory) super.clone();
	}

	@Override
	public String type() {
		return "Manual";
	}

}
