package cashpiles.model;

import cashpiles.ledger.CategoryTransactionEntry;

class Allocation extends ModelItem {

	private boolean active = false;
	private Category category;
	private CategoryTransactionEntry entry;

	Allocation(Category category, CategoryTransactionEntry entry) {
		this.category = category;
		this.entry = entry;
	}

	boolean active() {
		return active;
	}

	Category category() {
		return category;
	}

	CategoryTransactionEntry entry() {
		return entry;
	}

	Allocation makeActive() {
		var retval = clone();
		retval.active = true;
		return retval;
	}

	@Override
	public Allocation clone() {
		return (Allocation) super.clone();
	}

}
