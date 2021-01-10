package cashpiles.budget;

import java.util.Map;

import cashpiles.currency.Amount;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.time.DateRange;

class GoalCategory extends BudgetCategory {

	GoalCalculator calc;

	GoalCategory(String name, GoalCalculator calc, Amount startBalance, Map<String, Amount> owners, String owner) {
		super(name, startBalance, owners, owner);
		this.calc = calc.clone();
	}

	GoalCategory(String name, GoalBudgetEntry entry, Map<String, Amount> owners) {
		super(name, new Amount(), owners, entry.owner);
		calc = new GoalCalculator();
		calc.setAllocationDates(new DateRange(entry.parent.date, entry.parent.period));
		calc.setGoalAmount(entry.amount);
		calc.setGoalDates(entry.dates);
		calc.setGoalRepeat(entry.repeat);
	}

	@Override
	public BudgetCategory clone() {
		return new GoalCategory(name, calc, getBalance(), owners, owner);
	}

	@Override
	public Amount getAllocation() {
		return calc.getAllocationAmount();
	}

}
