package cashpiles.model2;

// this is an immutable data class
class Allocation extends ModelItem {

	private final Category category;

	Allocation(Category category) {
		this.category = category;
	}

	Category category() {
		return category;
	}

}
