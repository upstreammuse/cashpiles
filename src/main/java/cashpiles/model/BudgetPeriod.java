package cashpiles.model;

import cashpiles.time.DateRange;

class BudgetPeriod extends ModelItem {

	private final DateRange dates;

	BudgetPeriod(DateRange dates) {
		this.dates = dates;
	}

	DateRange dates() {
		return dates;
	}

	BudgetPeriod next() {
		return new BudgetPeriod(dates.next());
	}

	@Override
	public BudgetPeriod clone() {
		return (BudgetPeriod) super.clone();
	}

}
