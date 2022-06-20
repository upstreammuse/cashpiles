package cashpiles.model2;

// this is an immutable data class
class CrossAllocator extends ModelItem {

	private final Category category;

	CrossAllocator(Category category) {
		this.category = category;
	}

	Category allocate(Category category) {
		return category;
	}

	// TODO try removing this to see if Java can work out the correct method call
	ReserveCategory allocate(ReserveCategory category) {
		return category;
	}

	Category category() {
		return category;
	}

}
