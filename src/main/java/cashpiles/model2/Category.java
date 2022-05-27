package cashpiles.model2;

import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;

// this is an immutable data class
abstract class Category extends ModelItem {

	private final String owner;
	private List<CategoryTransactionEntry> transactions = new ArrayList<>();

	Category(String owner) {
		this.owner = owner;
	}

	Amount balance() {
		var retval = new Amount();
		for (var xact : transactions) {
			retval = retval.add(xact.amount());
		}
		return retval;
	}

	String owner() {
		return owner;
	}

	Category reset() {
		transactions.clear();
		return this;
	}

	Category withAllocation(CrossAllocator allocation) {
		return allocation.allocate(this);
	}

	CrossAllocator withTransaction(CategoryTransactionEntry entry) {
		var retval = clone();
		retval.transactions.add(entry);
		return new CrossAllocator(retval);
	}

	@Override
	public Category clone() {
		var retval = (Category) super.clone();
		retval.transactions = new ArrayList<>(retval.transactions);
		return retval;
	}

}
