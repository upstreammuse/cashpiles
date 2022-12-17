package cashpiles.model;

import java.util.ArrayList;

@SuppressWarnings("serial")
class PeriodsList extends ArrayList<BudgetPeriod> implements PeriodsView {

	PeriodsList() {
	}

	PeriodsList(PeriodsList other) {
		super(other);
	}

}
