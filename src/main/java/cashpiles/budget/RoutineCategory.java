package cashpiles.budget;

import cashpiles.currency.Amount;
import cashpiles.ledger.RoutineBudgetEntry;
import cashpiles.time.DateRange;

public class RoutineCategory extends BudgetCategory {

	DateRange currentRange;
	Amount priorActivity;
	DateRange priorRange;

	public RoutineCategory(String name, RoutineBudgetEntry entry) {
		super(name, entry.owner, new Amount());
		priorActivity = new Amount();
		priorRange = currentRange = new DateRange(entry.parent.date, entry.parent.period);
	}

	private RoutineCategory(String name, String owner, Amount amount) {
		super(name, owner, amount);
	}

	@Override
	public BudgetCategory next(DateRange dates) {
		var clone = new RoutineCategory(name, owner, getBalance());
		clone.priorActivity = priorActivity.add(getActivity());
		clone.priorRange = new DateRange(priorRange.startDate(), currentRange.endDate());
		clone.currentRange = dates;
		return clone;
	}

	@Override
	public Amount getAllocation() {
		// TODO this should be configurable instead of a hard-coded 180-day buffer.
		// that's too much for a lot of people to handle
		var perDay = priorActivity.negate().distributeAndAdd(priorRange,
				new DateRange(priorRange.startDate(), priorRange.startDate()));
		var comfortable = perDay.times(180).add(perDay.times(currentRange.numberOfDays()));
		return comfortable.add(startBalance.negate());
	}

	@Override
	public String typeString() {
		return "Routine";
	}

}
