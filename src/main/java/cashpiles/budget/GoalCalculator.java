package cashpiles.budget;

import java.time.DateTimeException;
import java.time.LocalDate;
import java.time.Period;
import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;
import cashpiles.util.Lists;

// this is an immutable data class
public class GoalCalculator implements Cloneable {

	private DateRange allocationDates = new DateRange(LocalDate.now(), Period.ofDays(1));
	private Amount goalAmount = new Amount();
	private DateRange goalDates = new DateRange(LocalDate.now(), Period.ofDays(1));;
	private boolean goalRepeat = false;

	public Amount allocationAmount() {
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

	@Override
	public GoalCalculator clone() {
		try {
			return (GoalCalculator) super.clone();
		} catch (CloneNotSupportedException ex) {
			throw new RuntimeException(ex);
		}
	}

	public boolean completed() {
		return !goalRepeat && goalDates.endDate().compareTo(allocationDates.endDate()) <= 0;
	}

	private List<DateRange> generateGoals() {
		var goals = new ArrayList<DateRange>();
		goals.add(goalDates);
		while (goalRepeat && Lists.lastOf(goals).endDate().compareTo(allocationDates.endDate()) < 0) {
			goals.add(Lists.lastOf(goals).next());
		}
		return goals;
	}

	public DateRange nextGoalDates() {
		if (completed()) {
			throw new DateTimeException("Goal dates are completed");
		}
		var goals = generateGoals();
		var nextDates = Lists.lastOf(goals);
		if (goalRepeat && nextDates.endDate().compareTo(allocationDates.endDate()) == 0) {
			nextDates = nextDates.next();
		}
		return nextDates;
	}

	public GoalCalculator withAllocationDates(DateRange dates) {
		var retval = clone();
		retval.allocationDates = dates;
		return retval;
	}

	public GoalCalculator withGoalAmount(Amount amount) {
		var retval = clone();
		retval.goalAmount = amount;
		return retval;
	}

	public GoalCalculator withGoalDates(DateRange dates) {
		var retval = clone();
		retval.goalDates = dates;
		return retval;
	}

	public GoalCalculator withGoalRepeat(boolean repeat) {
		var retval = clone();
		retval.goalRepeat = repeat;
		return retval;
	}

}
