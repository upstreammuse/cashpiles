package cashpiles.budget;

import java.util.Map;

import cashpiles.currency.Amount;
import cashpiles.ledger.WithholdingBudgetEntry;

public class WithholdingCategory extends BudgetCategory {

	public WithholdingCategory(String name, WithholdingBudgetEntry entry, Map<String, Amount> owners) {
		this(name, new Amount(), owners, entry.owner);
	}

	public WithholdingCategory(String name, Amount startBalance, Map<String, Amount> owners, String owner) {
		super(name, startBalance, owners, owner);
	}

	@Override
	public BudgetCategory clone() {
		return new WithholdingCategory(name, getBalance(), owners, owner);
	}

	@Override
	public Amount getAllocation() {
		return getActivity().negate();
	}

}
