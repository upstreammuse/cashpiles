package cashpiles.model;

import java.time.LocalDate;

import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.time.DateRange;

class IncomeCategory extends Category {

	IncomeCategory(LocalDate startDate, DateRange startPeriod, String owner) {
		super(startDate, startPeriod, owner);
	}

	@Override
	BudgetPeriod allocate(BudgetPeriod period) {
		// FIXME this needs to trigger each of the reserve categories to allocate based
		// on the transaction amount -> fortunately we know the transaction amount
		// because we always zero the balance, so whatever the outstanding balance here
		// is what we have to base the reserve % on
		return period.withAllocation(period.activity().negate());
	}

	@Override
	public IncomeCategory clone() {
		return (IncomeCategory) super.clone();
	}

	@Override
	public String type() {
		return "Income";
	}

	@Override
	public Allocation withTransaction(CategoryTransactionEntry entry) throws LedgerModelException {
		var retval = super.withTransaction(entry);
		retval = retval.makeActive();
		return retval;
	}

}
