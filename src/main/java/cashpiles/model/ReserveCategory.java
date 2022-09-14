package cashpiles.model;

import java.math.BigDecimal;

import cashpiles.currency.Amount;
import cashpiles.ledger.ReserveBudgetEntry;

//this is an immutable data class
class ReserveCategory extends Category {

	private final BigDecimal percentage;

	ReserveCategory(ReserveBudgetEntry entry) {
		super(entry.owner());
		percentage = entry.percentage();
	}

	@Override
	public ReserveCategory clone() {
		return (ReserveCategory) super.clone();
	}

	@Override
	public String type() {
		return "Reserve";
	}

	@Override
	Category withAllocation(CrossAllocator allocation) {
		return allocation.allocate(this);
	}

	ReserveCategory withIncomeAllocation(Amount amount) {
		var retval = clone();
		retval.allocation = retval.allocation.add(amount.percentage(percentage));
		return retval;
	}

}
