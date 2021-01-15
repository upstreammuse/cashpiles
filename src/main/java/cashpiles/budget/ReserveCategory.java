package cashpiles.budget;

import java.math.BigDecimal;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

public class ReserveCategory extends BudgetCategory {

	Amount allocation = new Amount();
	BigDecimal percentage;

	public ReserveCategory(String name, String owner, BigDecimal percentage) {
		this(name, owner, new Amount(), percentage);
	}

	public ReserveCategory(String name, String owner, Amount startBalance, BigDecimal percentage) {
		super(name, owner, startBalance);
		this.percentage = percentage;
	}

	@Override
	public BudgetCategory next(DateRange dates) {
		return new ReserveCategory(name, owner, getBalance(), percentage);
	}

	@Override
	public Amount getAllocation() {
		return allocation;
	}

	@Override
	public void link(BudgetCategory category) {
		category.link(name, this);
	}

	@Override
	public void link(String name, IncomeCategory category) {
		category.link(this.name, this);
	}

	public void reserve(Amount amount) {
		allocation = allocation.add(amount.percentage(percentage));
	}

	@Override
	public void unlink(BudgetCategory category) {
		category.unlink(name, this);
	}

	@Override
	public String typeString() {
		return "Reserve";
	}

}
