package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.time.DateRange;
import cashpiles.util.Lists;

// TODO this class needs to give access to the total allocation amount so that the ledger can calculate the owner balances
abstract class Category extends ModelItem implements CategoryView {

	private final String owner;
	private List<BudgetPeriod> periods = new ArrayList<>();
	private final LocalDate startDate;
	private final DateRange startDates;

	// TODO there is a separate start date for the possibility of allowing
	// categories to exist separately from a budget configuration, in which case the
	// category does not start on the same day as the budget period
	Category(LocalDate startDate, DateRange startDates, String owner) {
		assert (startDates.contains(startDate));
		this.owner = owner;
		this.startDate = startDate;
		this.startDates = startDates;
	}

	abstract BudgetPeriod allocate(BudgetPeriod period);

	@Override
	public Amount balance() {
		if (periods.isEmpty()) {
			return new Amount();
		} else {
			return Lists.lastOf(periods).balance();
		}
	}

	@Override
	public Category clone() {
		var retval = (Category) super.clone();
		retval.periods = new ArrayList<>(periods);
		if (retval.periods.isEmpty()) {
			retval.periods.add(retval.allocate(new BudgetPeriod(retval.startDates, new Amount())));
		}
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
		var retval = clone();
		if (entry.parent().date().compareTo(retval.periods.get(0).dates().startDate()) < 0) {
			throw LedgerModelException.forTooEarly(entry, retval.periods.get(0).dates().startDate());
		}
		while (Lists.lastOf(retval.periods).dates().endDate().compareTo(entry.parent().date()) < 0) {
			retval.periods.add(allocate(Lists.lastOf(retval.periods).next()));
		}
		return retval;
	}

	// TODO this method should be renamed to "withBudget" or something else to make
	// it more obvious that it's taking a new budget config, not just running out to
	// new periods
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
			retval.periods.add(allocate(Lists.lastOf(retval.periods).next()));
		}

		// then configure the new period
		retval.periods.add(allocate(new BudgetPeriod(new DateRange(budget.date(), budget.period()),
				Lists.lastOf(retval.periods).balance())));

		return retval;
	}

	Category withTransaction(CategoryTransactionEntry entry) throws LedgerModelException {
		var retval = withDate(entry);
		var period = Lists.lastOf(retval.periods);
		retval.periods.remove(retval.periods.size() - 1);
		period = period.withTransaction(entry);
		period = allocate(period);
		retval.periods.add(period);
		return retval;
	}

}
