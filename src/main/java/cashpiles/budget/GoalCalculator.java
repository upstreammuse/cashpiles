package cashpiles.budget;

import java.time.DateTimeException;
import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;
import cashpiles.util.Lists;

class GoalCalculator implements Cloneable {

	private DateRange allocationDates;
	private Amount goalAmount;
	private DateRange goalDates;
	private boolean goalRepeat;
	
	@Override
	public GoalCalculator clone() {
		GoalCalculator result = new GoalCalculator();
		result.setAllocationDates(allocationDates);
		result.setGoalAmount(goalAmount);
		result.setGoalDates(goalDates);
		result.setGoalRepeat(goalRepeat);
		return result;
	}

	Amount getAllocationAmount() {
		var goals = generateGoals();
		var result = new Amount();
		for (var goal : goals) {
			try {
				result = result.add(goalAmount.distributeAndAdd(goal, goal.intersect(allocationDates)));
			} catch (DateTimeException ex) {
			}
		}
		return result;
	}

	boolean getCompleted() {
		return !goalRepeat && goalDates.endDate().compareTo(allocationDates.endDate()) <= 0;
	}

	DateRange getNextGoalDates() {
		if (getCompleted()) {
			throw new DateTimeException("Goal dates are completed");
		}
		var goals = generateGoals();
		var nextDates = Lists.lastOf(goals);
		if (goalRepeat && nextDates.endDate().compareTo(allocationDates.endDate()) == 0) {
			nextDates = nextDates.next();
		}
		return nextDates;
	}

	void setAllocationDates(DateRange dates) {
		allocationDates = dates;
	}

	void setGoalAmount(Amount amount) {
		goalAmount = amount;
	}

	void setGoalDates(DateRange dates) {
		goalDates = dates;
	}

	void setGoalRepeat(boolean repeat) {
		goalRepeat = repeat;
	}

	private List<DateRange> generateGoals() {
		var goals = new ArrayList<DateRange>();
		goals.add(goalDates);
		while (goalRepeat && Lists.lastOf(goals).endDate().compareTo(allocationDates.endDate()) < 0) {
			goals.add(Lists.lastOf(goals).next());
		}
		return goals;
	}

}
