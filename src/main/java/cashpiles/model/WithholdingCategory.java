package cashpiles.model;

import java.time.LocalDate;

import cashpiles.time.DateRange;

class WithholdingCategory extends Category {

	WithholdingCategory(LocalDate startDate, DateRange startPeriod, String owner) {
		super(startDate, startPeriod, owner);
	}

	@Override
	BudgetPeriod allocate(BudgetPeriod period) {
		return period.withAllocation(period.activity().negate());
	}

	@Override
	public WithholdingCategory clone() {
		return (WithholdingCategory) super.clone();
	}

	@Override
	public String type() {
		return "Withholding";
	}

}
