package cashpiles.budget;

import cashpiles.currency.Amount;
import cashpiles.ledger.WithholdingBudgetEntry;
import cashpiles.time.DateRange;

public class WithholdingCategory extends BudgetCategory {

	public WithholdingCategory(String name, WithholdingBudgetEntry entry) {
		this(name, entry.owner(), new Amount());
	}

	public WithholdingCategory(String name, String owner, Amount startBalance) {
		super(name, owner, startBalance);
	}

	@Override
	public BudgetCategory next(DateRange dates) {
		return new WithholdingCategory(name, owner, getBalance());
	}

	@Override
	public Amount getAllocation() {
		return getActivity().negate();
	}

	@Override
	public String typeString() {
		return "Withholding";
	}

}
