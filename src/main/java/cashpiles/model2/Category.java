package cashpiles.model2;

import cashpiles.currency.Amount;

// this is an immutable data class
class Category extends ModelItem {

	public Amount balance() {
		return new Amount();
	}

	Category reset() {
		return this;
	}

}
