package cashpiles.model;

import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.WithholdingBudgetEntry;

class WithholdingCategory extends Category {

	WithholdingCategory(WithholdingBudgetEntry entry) {
		super(entry.owner());
	}

	@Override
	public String type() {
		return "Withholding";
	}

	@Override
	CrossAllocator withTransaction(CategoryTransactionEntry entry) {
		var alloc = super.withTransaction(entry);
		var category = alloc.category();
		category.allocation = category.allocation.add(category.balance().negate());
		return new CrossAllocator(category);
	}

}
