package cashpiles.budget;

import java.util.HashMap;
import java.util.Map;

import cashpiles.currency.Amount;
import cashpiles.ledger.GoalBudgetEntry;

class GoalsCategory extends BudgetCategory {

	Map<String, GoalCalculator> calcs = new HashMap<>();

	GoalsCategory(GoalBudgetEntry entry, Map<String, Amount> owners) {
		super(new Amount(), owners, entry.owner);
	}

	GoalsCategory(Amount startBalance, Map<String, Amount> owners, String owner) {
		super(startBalance, owners, owner);
	}

	@Override
	public BudgetCategory clone() {
		var dup = new GoalsCategory(getBalance(), owners, owner);
		for (var calc : calcs.entrySet()) {
			if (!calc.getValue().getCompleted()) {
				dup.calcs.put(calc.getKey(), calc.getValue().clone());
			}
		}
		return dup;
	}

	public Amount getAllocation() {
		return calcs.entrySet().stream().map(s -> s.getValue().getAllocationAmount()).reduce(new Amount(), Amount::add);
	}

}
