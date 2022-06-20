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
	private Amount startBalance;
	protected List<CategoryTransactionEntry> transactions = new ArrayList<>();

	Category(String owner) {
		this.owner = owner;
		this.startBalance = new Amount();
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
		return startBalance.add(allocation).add(activity());
	}

	@Override
	public Category clone() {
		var retval = (Category) super.clone();
		retval.transactions = new ArrayList<>(retval.transactions);
		return retval;
	}

	Category next(DateRange dates) {
		var retval = clone();
		retval.allocation = new Amount();
		retval.startBalance = balance();
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
