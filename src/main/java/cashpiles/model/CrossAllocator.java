package cashpiles.model;

// this is an immutable data class
class CrossAllocator extends ModelItem {

	private final Category category;

	CrossAllocator(Category category) {
		this.category = category;
	}

	Category allocate(Category category) {
		return category;
	}

	ReserveCategory allocate(ReserveCategory category) {
		return category;
	}

	Category category() {
		return category;
	}

}
