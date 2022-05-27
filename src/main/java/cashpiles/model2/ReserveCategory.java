package cashpiles.model2;

import java.math.BigDecimal;

import cashpiles.currency.Amount;
import cashpiles.ledger.ReserveBudgetEntry;

class ReserveCategory extends Category {

	private final BigDecimal percentage;

	ReserveCategory(ReserveBudgetEntry entry) {
		super(entry.owner());
		percentage = entry.percentage();
	}

	ReserveCategory withIncomeAllocation(Amount amount) {
		var retval = clone();
		retval.allocation = retval.allocation.add(amount.percentage(percentage));
		return retval;
	}

	@Override
	public ReserveCategory clone() {
		return (ReserveCategory) super.clone();
	}

}
