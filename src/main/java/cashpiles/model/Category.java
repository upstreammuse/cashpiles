package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import cashpiles.ledger.Budget;
import cashpiles.time.DateRange;
import cashpiles.util.Lists;

class Category extends ModelItem {

	private List<DateRange> periods = new ArrayList<>();
	private final LocalDate startDate;

	Category(LocalDate startDate, DateRange startPeriod) {
		assert (startPeriod.contains(startDate));
		this.startDate = startDate;
		periods.add(startPeriod);
	}

	public Category withPeriods(Budget budget) throws LedgerModelException {
		var retval = clone();

		// make sure new budget will align with current cycle
		var nextPeriod = Lists.lastOf(retval.periods).next();
		while (nextPeriod.startDate().compareTo(budget.date()) < 0) {
			nextPeriod = nextPeriod.next();
		}
		if (!nextPeriod.startDate().equals(budget.date())) {
			throw LedgerModelException.forBudgetStartDate(budget, nextPeriod.startDate());
		}

		// create any needed periods on the current cycle
		while (!Lists.lastOf(retval.periods).next().startDate().equals(budget.date())) {
			retval.periods.add(Lists.lastOf(retval.periods).next());
		}

		// then configure the new period
		retval.periods.add(new DateRange(budget.date(), budget.period()));

		return retval;
	}

	@Override
	public Category clone() {
		var retval = (Category) super.clone();
		retval.periods = new ArrayList<>(periods);
		return retval;
	}

}
