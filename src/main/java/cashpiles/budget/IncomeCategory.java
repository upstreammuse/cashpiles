package cashpiles.budget;

import java.util.HashMap;
import java.util.Map;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.time.DateRange;

public class IncomeCategory extends BudgetCategory {

	Map<String, ReserveCategory> reserves = new HashMap<>();

	public IncomeCategory(String name, String owner) {
		super(name, owner, new Amount());
	}

	@Override
	void addTransaction(CategoryTransactionEntry transaction) {
		super.addTransaction(transaction);
		for (var reserve : reserves.entrySet()) {
			reserve.getValue().reserve(transaction.amount());
		}
	}

	@Override
	public Amount getAllocation() {
		return getActivity().negate();
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
	public BudgetCategory next(DateRange dates) {
		return new IncomeCategory(name, owner);
	}

	@Override
	public void unlink(BudgetCategory category) {
		category.unlink(this.name, this);
	}

	@Override
	public void unlink(String name, ReserveCategory category) {
		reserves.remove(name);
	}

	@Override
	public String typeString() {
		return "Income";
	}

}
