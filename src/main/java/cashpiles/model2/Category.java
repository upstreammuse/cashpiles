package cashpiles.model2;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;

// this is an immutable data class
class Category extends ModelItem {

	Amount balance() {
		return new Amount();
	}

	String owner() {
		return "";
	}

	Category reset() {
		return this;
	}

	Category withTransaction(CategoryTransactionEntry entry) {
		return this;
	}

}
