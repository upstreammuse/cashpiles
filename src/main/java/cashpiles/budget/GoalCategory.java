package cashpiles.budget;

import cashpiles.currency.Amount;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.time.DateRange;

class GoalCategory extends BudgetCategory {

	GoalCalculator calc;

	GoalCategory(String name, GoalCalculator calc, Amount startBalance, String owner) {
		super(name, owner, startBalance);
		this.calc = calc;
	}

	GoalCategory(String name, GoalBudgetEntry entry) {
		super(name, entry.owner, new Amount());
		calc = new GoalCalculator();
		calc.setAllocationDates(new DateRange(entry.parent.date, entry.parent.period));
		calc.setGoalAmount(entry.amount);
		calc.setGoalDates(entry.dates);
		calc.setGoalRepeat(entry.repeat);
	}

	@Override
	public BudgetCategory next(DateRange dates) {
		var clone = new GoalCategory(name, calc.clone(), getBalance(), owner);
		clone.calc.setAllocationDates(dates);
		return clone;
	}

	@Override
	public Amount getAllocation() {
		return calc.getAllocationAmount();
	}

	@Override
	public String typeString() {
		return "Goal";
	}

}
