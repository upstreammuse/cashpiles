package cashpiles.model2;

import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.time.DateRange;

// this is an immutable data class
abstract class Category extends ModelItem implements CategoryView {

	protected Amount allocation = new Amount();
	private final String owner;
	private Amount startActivity;
	private Amount startAllocation;
	protected List<CategoryTransactionEntry> transactions = new ArrayList<>();

	Category(String owner) {
		this.owner = owner;
		this.startActivity = new Amount();
		this.startAllocation = new Amount();
	}

	@Override
	public Amount activity() {
		var total = new Amount();
		for (var xact : transactions) {
			total = total.add(xact.amount());
		}
		return total;
	}

	@Override
	public Amount allocation() {
		return allocation;
	}

	@Override
	public Amount balance() {
		return startAllocation.add(startActivity).add(allocation).add(activity());
	}

	@Override
	public Category clone() {
		var retval = (Category) super.clone();
		retval.transactions = new ArrayList<>(retval.transactions);
		return retval;
	}

	@Override
	public Amount lifetimeAllocation() {
		return startAllocation.add(allocation);
	}

	Category next(DateRange dates) {
		var retval = clone();
		retval.allocation = new Amount();
		retval.startActivity = startActivity.add(activity());
		retval.startAllocation = startAllocation.add(allocation);
		retval.transactions.clear();
		return retval;
	}

	@Override
	public String owner() {
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
