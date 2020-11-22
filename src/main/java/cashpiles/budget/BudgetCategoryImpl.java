package cashpiles.budget;

import cashpiles.currency.Amount;

public abstract class BudgetCategoryImpl implements BudgetCategory {

	private boolean active = true;
	protected final Amount startBalance;

	public BudgetCategoryImpl(Amount startBalance) {
		this.startBalance = startBalance;
	}

	@Override
	public abstract BudgetCategory clone();

	@Override
	public abstract Amount getBalance();

	@Override
	public boolean isActive() {
		return active;
	}

	@Override
	public void setActive(boolean active) {
		this.active = active;
	}

}
