package cashpiles.budget;

import java.math.BigDecimal;
import java.util.Map;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

public class ReserveCategory extends BudgetCategory {

	Amount allocation = new Amount();
	BigDecimal percentage;

	public ReserveCategory(String name, Map<String, Amount> owners, String owner, BigDecimal percentage) {
		this(name, new Amount(), owners, owner, percentage);
	}

	public ReserveCategory(String name, Amount startBalance, Map<String, Amount> owners, String owner,
			BigDecimal percentage) {
		super(name, startBalance, owners, owner);
		this.percentage = percentage;
	}

	@Override
	public BudgetCategory next(DateRange dates) {
		return new ReserveCategory(name, getBalance(), owners, owner, percentage);
	}

	@Override
	public Amount getAllocation() {
		return allocation;
	}

	@Override
	public void link(BudgetCategory category) {
		category.link(name, this);
	}

	public void reserve(Amount amount) {
		allocation = allocation.add(amount.percentage(percentage));
	}

	@Override
	public void unlink(BudgetCategory category) {
		category.unlink(name, this);
	}

}
