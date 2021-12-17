package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class ReserveCategory extends Category {

	ReserveCategory(LocalDate startDate, DateRange startPeriod, String owner) {
		super(startDate, startPeriod, owner);
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

}
