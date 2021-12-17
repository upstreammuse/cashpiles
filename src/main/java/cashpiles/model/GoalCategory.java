package cashpiles.model;

import cashpiles.budget.GoalCalculator;
import cashpiles.currency.Amount;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.time.DateRange;

class GoalCategory extends Category {

	private final GoalBudgetEntry entry;

	GoalCategory(GoalBudgetEntry entry) {
		super(entry.dates().startDate(), new DateRange(entry.parent().date(), entry.parent().period()), entry.owner());
		this.entry = entry;
	}

	@Override
	BudgetPeriod allocate(BudgetPeriod period) {
		var allocation = period.allocation();
		if (allocation.equals(new Amount())) {
			var calc = new GoalCalculator();
			calc.setAllocationDates(period.dates());
			calc.setGoalAmount(entry.amount());
			calc.setGoalDates(entry.dates());
			calc.setGoalRepeat(entry.repeat());
			period = period.withAllocation(calc.getAllocationAmount());
		}
		return period;
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
