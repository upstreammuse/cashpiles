package cashpiles.model;

import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.IncomeBudgetEntry;

// this is an immutable data class
class IncomeCategory extends Category {

	IncomeCategory(IncomeBudgetEntry entry) {
		super(entry.owner());
	}

	@Override
	public String type() {
		return "Income";
	}

	@Override
	CrossAllocator withTransaction(CategoryTransactionEntry entry) {
		var alloc = super.withTransaction(entry);
		var category = alloc.category();
		category.allocation = category.allocation.add(category.balance().negate());
		return new IncomeAllocator(category, entry.amount());
	}

}
