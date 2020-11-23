package cashpiles.budget;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import cashpiles.currency.Amount;

class GoalsCategory extends BudgetCategoryImpl {

	class BudgetTransaction {
		Amount amount;
		String payee;
	}

	Map<String, GoalCalculator> calcs = new HashMap<>();
	List<BudgetTransaction> transactions = new ArrayList<>();

	GoalsCategory(Amount startBalance) {
		super(startBalance);
	}

	void addTransaction(BudgetTransaction transaction) {
		transactions.add(transaction);
	}

	@Override
	public BudgetCategory clone() {
		var dup = new GoalsCategory(getBalance());
		// TODO figure out stream version for learning sake
		for (var calc : calcs.entrySet()) {
			if (!calc.getValue().getCompleted()) {
				dup.calcs.put(calc.getKey(), calc.getValue().clone());
			}
		}
		return dup;
	}

	@Override
	public Amount getActivity() {
		return transactions.stream().map(t -> t.amount).reduce(new Amount(), Amount::add);
	}

	@Override
	public Amount getAllocation() {
		return calcs.entrySet().stream().map(s -> s.getValue().getAllocationAmount()).reduce(new Amount(), Amount::add);
	}

	@Override
	public Amount getBalance() {
		return getAllocation().add(getActivity());
	}

}
