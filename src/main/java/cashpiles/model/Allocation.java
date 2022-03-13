package cashpiles.model;

import cashpiles.ledger.CategoryTransactionEntry;

class Allocation extends ModelItem {

	private Category category;
	private CategoryTransactionEntry entry;

	Allocation(Category category, CategoryTransactionEntry entry) {
		this.category = category;
		this.entry = entry;
	}

	Category allocate(Category category) {
		return category;
	}

	Category allocate(ReserveCategory category) throws LedgerModelException {
		return category;
	}

	Category category() {
		return category;
	}

	CategoryTransactionEntry entry() {
		return entry;
	}

	@Override
	public Allocation clone() {
		return (Allocation) super.clone();
	}

}
