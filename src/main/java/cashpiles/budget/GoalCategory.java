package cashpiles.budget;

import java.util.Map;

import cashpiles.currency.Amount;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.time.DateRange;

class GoalCategory extends BudgetCategory {

	GoalCalculator calc;

	GoalCategory(String name, GoalCalculator calc, Amount startBalance, Map<String, Amount> owners, String owner) {
		super(name, startBalance, owners, owner);
		// FIXME the calc isn't getting updated allocation dates, only when configured
		this.calc = calc.clone();
	}

	GoalCategory(String name, GoalBudgetEntry entry, Map<String, Amount> owners) {
		super(name, new Amount(), owners, entry.owner);
		calc = new GoalCalculator();
		// FIXME the calc isn't getting updated allocation dates, only when configured
		calc.setAllocationDates(new DateRange(entry.parent.date, entry.parent.period));
		calc.setGoalAmount(entry.amount);
		calc.setGoalDates(entry.dates);
		calc.setGoalRepeat(entry.repeat);
	}

	@Override
	public BudgetCategory next(DateRange dates) {
		// FIXME the calc isn't getting new allocation dates, but should be able to get them here
		return new GoalCategory(name, calc, getBalance(), owners, owner);
	}

	@Override
	public Amount getAllocation() {
		return calc.getAllocationAmount();
	}

}
