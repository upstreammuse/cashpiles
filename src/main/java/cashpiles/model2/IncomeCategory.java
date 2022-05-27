package cashpiles.model2;

import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.IncomeBudgetEntry;

// this is an immutable data class
class IncomeCategory extends Category {

	IncomeCategory(IncomeBudgetEntry entry) {
		super(entry.owner());
	}

	@Override
	CrossAllocator withTransaction(CategoryTransactionEntry entry) {
		var alloc = super.withTransaction(entry);
		return new IncomeAllocator(alloc.category());
	}

}
