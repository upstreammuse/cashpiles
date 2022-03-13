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
		var alloc = super.withTransaction(entry);
		return new IncomeAllocation(alloc.category(), alloc.entry());
	}

}
