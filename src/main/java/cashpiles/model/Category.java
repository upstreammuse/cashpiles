package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.time.DateRange;
import cashpiles.util.Lists;

abstract class Category extends ModelItem implements CategoryView {

	private final String owner;
	private List<BudgetPeriod> periods = new ArrayList<>();
	private final LocalDate startDate;

	// TODO there is a separate start date for the possibility of allowing
	// categories to exist separately from a budget configuration, in which case the
	// category does not start on the same day as the budget period
	Category(LocalDate startDate, DateRange startPeriod, String owner) {
		assert (startPeriod.contains(startDate));
		this.owner = owner;
		this.startDate = startDate;
		periods.add(new BudgetPeriod(startPeriod, new Amount()));
	}

	@Override
	public Amount balance() {
		return Lists.lastOf(periods).balance();
	}

	@Override
	public Category clone() {
		var retval = (Category) super.clone();
		retval.periods = new ArrayList<>(periods);
		return retval;
	}

	@Override
	public PeriodView get(int index) {
		return periods.get(index);
	}

	@Override
	public String owner() {
		return owner;
	}

	@Override
	public int size() {
		return periods.size();
	}

	@Override
	abstract public String type();

	Category withDate(CategoryTransactionEntry entry) throws LedgerModelException {
		if (entry.parent().date().compareTo(periods.get(0).dates().startDate()) < 0) {
			throw LedgerModelException.forTooEarly(entry, periods.get(0).dates().startDate());
		}
		var retval = clone();
		while (Lists.lastOf(retval.periods).dates().endDate().compareTo(entry.parent().date()) < 0) {
			retval.periods.add(Lists.lastOf(retval.periods).next());
		}
		return retval;
	}

	Category withPeriods(Budget budget) throws LedgerModelException {
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
		retval.periods.add(new BudgetPeriod(new DateRange(budget.date(), budget.period()),
				Lists.lastOf(retval.periods).balance()));

		return retval;
	}

	Category withTransaction(CategoryTransactionEntry entry) throws LedgerModelException {
		// withDate guarantees that the category contains the requested date or throws,
		// so we will find a matching period inside the loop
		var retval = withDate(entry);
		for (var it = retval.periods.listIterator(); it.hasNext(); /* inside */) {
			var period = it.next();
			if (period.dates().contains(entry.parent().date())) {
				it.set(period.withTransaction(entry));
				return retval;
			}
		}
		assert (false) : "withTransaction failed to find matching budget period";
		return null;
	}

}
