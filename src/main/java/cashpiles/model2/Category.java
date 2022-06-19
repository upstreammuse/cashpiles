package cashpiles.model2;

import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.time.DateRange;

// this is an immutable data class
abstract class Category extends ModelItem {

	protected Amount allocation = new Amount();
	private final String owner;
	protected List<CategoryTransactionEntry> transactions = new ArrayList<>();

	Category(String owner) {
		this.owner = owner;
	}

	Amount balance() {
		var retval = allocation;
		for (var xact : transactions) {
			retval = retval.add(xact.amount());
		}
		return retval;
	}

	@Override
	public Category clone() {
		var retval = (Category) super.clone();
		retval.transactions = new ArrayList<>(retval.transactions);
		return retval;
	}

	Category next(DateRange dates) {
		var retval = clone();
		retval.transactions.clear();
		return retval;
	}

	String owner() {
		return owner;
	}

	Category withAllocation(CrossAllocator allocation) {
		return allocation.allocate(this);
	}

	CrossAllocator withTransaction(CategoryTransactionEntry entry) {
		var retval = clone();
		retval.transactions.add(entry);
		return new CrossAllocator(retval);
	}

}
