package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import cashpiles.ledger.Budget;
import cashpiles.time.DateRange;
import cashpiles.util.Lists;

class Category extends ModelItem {

	private List<BudgetPeriod> periods = new ArrayList<>();
	private final LocalDate startDate;

	// TODO there is a separate start date for the possibility of allowing
	// categories to exist separately from a budget configuration, in which case the
	// category does not start on the same day as the budget period
	Category(LocalDate startDate, DateRange startPeriod) {
		assert (startPeriod.contains(startDate));
		this.startDate = startDate;
		periods.add(new BudgetPeriod(startPeriod));
	}

	public Category withPeriods(Budget budget) throws LedgerModelException {
		var retval = clone();

		// make sure new budget will align with current cycle
		var nextPeriod = Lists.lastOf(retval.periods).next();
		while (nextPeriod.dates().startDate().compareTo(budget.date()) < 0) {
			nextPeriod = nextPeriod.next();
		}
		if (!nextPeriod.dates().startDate().equals(budget.date())) {
			throw LedgerModelException.forBudgetStartDate(budget, nextPeriod.dates().startDate());
		}

		// create any needed periods on the current cycle
		while (!Lists.lastOf(retval.periods).next().dates().startDate().equals(budget.date())) {
			retval.periods.add(Lists.lastOf(retval.periods).next());
		}

		// then configure the new period
		retval.periods.add(new BudgetPeriod(new DateRange(budget.date(), budget.period())));

		return retval;
	}

	@Override
	public Category clone() {
		var retval = (Category) super.clone();
		retval.periods = new ArrayList<>(periods);
		return retval;
	}

}
