package cashpiles.budget;

import cashpiles.currency.Amount;
import cashpiles.ledger.ManualGoalBudgetEntry;
import cashpiles.time.DateRange;

public class ManualGoalCategory extends BudgetCategory {

	public ManualGoalCategory(String name, ManualGoalBudgetEntry entry) {
		this(name, entry.owner, new Amount());
	}

	public ManualGoalCategory(String name, String owner, Amount amount) {
		super(name, owner, amount);
	}

	@Override
	public BudgetCategory next(DateRange dates) {
		return new ManualGoalCategory(name, owner, getBalance());
	}

	@Override
	public Amount getAllocation() {
		return new Amount();
	}

	@Override
	public String typeString() {
		return "Goal";
	}

}
