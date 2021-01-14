package cashpiles.budget;

import java.util.Map;

import cashpiles.currency.Amount;
import cashpiles.ledger.RoutineBudgetEntry;
import cashpiles.time.DateRange;

public class RoutineCategory extends BudgetCategory {

	Amount priorActivity;
	DateRange priorRange;

	// TODO I would rather get this from the parent budget period instead of having
	// to redundantly track it here
	DateRange currentRange;

	public RoutineCategory(String name, RoutineBudgetEntry entry, Map<String, Amount> owners) {
		super(name, new Amount(), owners, entry.owner);
		priorActivity = new Amount();
		priorRange = currentRange = new DateRange(entry.parent.date, entry.parent.period);
	}

	private RoutineCategory(String name, Amount amount, Map<String, Amount> owners, String owner) {
		super(name, amount, owners, owner);
	}

	@Override
	public BudgetCategory next(DateRange dates) {
		var clone = new RoutineCategory(name, getBalance(), owners, owner);
		clone.priorActivity = priorActivity.add(getActivity());
		clone.priorRange = new DateRange(priorRange.startDate(), currentRange.endDate());
		clone.currentRange = currentRange.next();
		return clone;
	}

	@Override
	public Amount getAllocation() {
		var perDay = priorActivity.distributeAndAdd(priorRange,
				new DateRange(priorRange.startDate(), priorRange.startDate()));
		return perDay.times(currentRange.numberOfDays());
	}

	@Override
	public String typeString() {
		return "Routine";
	}

}
