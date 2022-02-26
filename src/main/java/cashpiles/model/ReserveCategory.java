package cashpiles.model;

import java.math.BigDecimal;
import java.time.LocalDate;

import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.time.DateRange;
import cashpiles.util.Lists;

class ReserveCategory extends Category {

	private BigDecimal percentage;

	ReserveCategory(LocalDate startDate, DateRange startPeriod, BigDecimal percentage, String owner) {
		super(startDate, startPeriod, owner);
		this.percentage = percentage;
	}

	@Override
	BudgetPeriod allocate(BudgetPeriod period) {
		// FIXME reserves don't allocate based on transactions, but they allocate based
		// on transactions from Incomes, so Reserve and Income need to know about each
		// other somehow
		return period;
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
	ReserveCategory withAllocation(Allocation allocation) throws LedgerModelException {
		var retval = withDate(allocation.entry());
		if (!allocation.active()) {
			return retval;
		}
		var lastPeriod = Lists.lastOf(retval.periods);
		retval.periods.remove(retval.periods.size() - 1);
		retval.periods.add(lastPeriod.withAllocation(
				lastPeriod.allocation().add(allocation.entry().amount().percentage(retval.percentage))));
		return retval;
	}

	@Override
	ReserveCategory withDate(CategoryTransactionEntry entry) throws LedgerModelException {
		return (ReserveCategory) super.withDate(entry);
	}

}
