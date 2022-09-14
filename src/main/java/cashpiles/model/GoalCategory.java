package cashpiles.model;

import cashpiles.budget.GoalCalculator;
import cashpiles.currency.Amount;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.time.DateRange;

// this is an immutable data class
class GoalCategory extends Category {

	private final Amount amount;
	private final DateRange goalDates;
	private final boolean repeat;

	GoalCategory(GoalBudgetEntry entry) {
		super(entry.owner());
		amount = entry.amount();
		goalDates = entry.dates();
		repeat = entry.repeat();
		allocation = allocation(new DateRange(entry.parent().date(), entry.parent().period()));
	}

	private Amount allocation(DateRange dates) {
		var calc = new GoalCalculator();
		calc = calc.withAllocationDates(dates);
		calc = calc.withGoalAmount(amount);
		calc = calc.withGoalDates(goalDates);
		calc = calc.withGoalRepeat(repeat);
		return calc.allocationAmount();
	}

	@Override
	GoalCategory next(DateRange dates) {
		var retval = (GoalCategory) super.next(dates);
		retval.allocation = retval.allocation.add(retval.allocation(dates));
		return retval;
	}

	@Override
	public String type() {
		return "Goal";
	}

}
