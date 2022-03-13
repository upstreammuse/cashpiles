package cashpiles.model;

import java.math.BigDecimal;
import java.time.LocalDate;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.time.DateRange;

class ReserveCategory extends Category {

	private BigDecimal percentage;

	ReserveCategory(LocalDate startDate, DateRange startPeriod, BigDecimal percentage, String owner) {
		super(startDate, startPeriod, owner);
		this.percentage = percentage;
	}

	public ReserveCategory allocate(CategoryTransactionEntry entry) throws LedgerModelException {
		var retval = withDate(entry);
		retval = retval.withAllocation(
				retval.get(retval.size() - 1).allocation().add(entry.amount().percentage(retval.percentage)));
		return retval;
	}

	@Override
	Category allocate(Allocation allocation) throws LedgerModelException {
		return allocation.allocate(this);
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
	ReserveCategory withAllocation(Amount amount) throws LedgerModelException {
		return (ReserveCategory) super.withAllocation(amount);
	}

	@Override
	ReserveCategory withDate(CategoryTransactionEntry entry) throws LedgerModelException {
		return (ReserveCategory) super.withDate(entry);
	}

}
