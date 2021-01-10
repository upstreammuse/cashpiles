package cashpiles.budget;

import java.util.Map;

import cashpiles.currency.Amount;
import cashpiles.ledger.ManualGoalBudgetEntry;

public class ManualGoalCategory extends BudgetCategory {

	public ManualGoalCategory(String name, ManualGoalBudgetEntry entry, Map<String, Amount> owners) {
		this(name, new Amount(), owners, entry.owner);
	}

	public ManualGoalCategory(String name, Amount amount, Map<String, Amount> owners, String owner) {
		super(name, amount, owners, owner);
	}

	@Override
	public BudgetCategory clone() {
		return new ManualGoalCategory(name, getBalance(), owners, owner);
	}

	@Override
	public Amount getAllocation() {
		return new Amount();
	}

}
