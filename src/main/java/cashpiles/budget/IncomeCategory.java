package cashpiles.budget;

import java.util.HashMap;
import java.util.Map;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;

public class IncomeCategory extends BudgetCategory {

	Map<String, ReserveCategory> reserves = new HashMap<>();

	public IncomeCategory(String name, Map<String, Amount> owners, String owner) {
		super(name, new Amount(), owners, owner);
	}

	@Override
	void addTransaction(CategoryTransactionEntry transaction) {
		super.addTransaction(transaction);
		for (var reserve : reserves.entrySet()) {
			reserve.getValue().reserve(transaction.amount);
		}
	}

	@Override
	public BudgetCategory clone() {
		var clone = new IncomeCategory(name, owners, owner);
		for (var reserve : reserves.entrySet()) {
			// TODO this feels so wrong having to do it this way
			clone.reserves.put(reserve.getKey(), (ReserveCategory) reserve.getValue().clone());
		}
		return clone;
	}

	@Override
	public Amount getAllocation() {
		return new Amount();
	}

	@Override
	public void link(BudgetCategory category) {
		category.link(this.name, this);
	}

	@Override
	public void link(String name, ReserveCategory category) {
		reserves.put(name, category);
	}

	@Override
	public void unlink(BudgetCategory category) {
		category.unlink(this.name, this);
	}

	@Override
	public void unlink(String name, ReserveCategory category) {
		reserves.remove(name);
	}

}
