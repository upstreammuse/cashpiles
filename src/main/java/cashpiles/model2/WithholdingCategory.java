package cashpiles.model2;

import cashpiles.ledger.WithholdingBudgetEntry;

class WithholdingCategory extends Category {

	WithholdingCategory(WithholdingBudgetEntry entry) {
		super(entry.owner());
	}

	@Override
	public String type() {
		return "Withholding";
	}

}
