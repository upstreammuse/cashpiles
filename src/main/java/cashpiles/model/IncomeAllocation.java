package cashpiles.model;

import cashpiles.ledger.CategoryTransactionEntry;

class IncomeAllocation extends Allocation {

	IncomeAllocation(Category category, CategoryTransactionEntry entry) {
		super(category, entry);
	}

	@Override
	Category allocate(ReserveCategory category) throws LedgerModelException {
		return category.allocate(entry());
	}

}
