package cashpiles.model2;

import java.util.Map;
import java.util.Optional;
import java.util.TreeMap;

import cashpiles.currency.Amount;
import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.ledger.IncomeBudgetEntry;
import cashpiles.ledger.ManualGoalBudgetEntry;
import cashpiles.ledger.ReserveBudgetEntry;
import cashpiles.time.DateRange;

// this is an immutable data class
class BudgetPeriod extends ModelItem {

	// the categories in this budget period
	private Map<String, Category> categories = new TreeMap<>();

	// the dates covered by this period
	private DateRange dates;

	// the unallocated balances of each owner as of this budget period
	private Map<String, Amount> owners = new TreeMap<>();

	// create the first budget period
	BudgetPeriod(Budget budget) {
		dates = new DateRange(budget.date(), budget.period());
	}

	BudgetPeriod close(CloseBudgetEntry entry) throws ModelException {
		// make the new value if we have what is needed
		if (!categories.containsKey(entry.name())) {
			throw ModelException.forUnknown(entry);
		}
		var retval = clone();

		// clear the category out of the new value
		var category = retval.categories.remove(entry.name());

		// modify the owner balance of the new value
		var ownerBalance = Optional.ofNullable(retval.owners.get(category.owner())).orElse(new Amount());
		ownerBalance = ownerBalance.add(category.balance());
		retval.owners.put(category.owner(), ownerBalance);

		// provide the new value
		return retval;
	}

	// get the dates of the period
	DateRange dates() {
		return dates;
	}

	// generate the next budget period based on the current one, which allows us to
	// continue the budget cycle without explicit budget entries in the file
	BudgetPeriod next() {
		return next(dates.next());
	}

	// generate the next budget period based on the current one, but use a different
	// duration for the new period
	BudgetPeriod next(Budget budget) throws ModelException {
		if (!dates.next().startDate().equals(budget.date())) {
			throw ModelException.forOutOfSync(budget);
		}
		return next(new DateRange(budget.date(), budget.period()));
	}

	// common code for both instances of next period generation
	private BudgetPeriod next(DateRange dates) {
		var retval = clone();
		retval.dates = dates;
		for (var entry : retval.categories.entrySet()) {
			entry.setValue(entry.getValue().next(retval.dates));
		}
		return retval;
	}

	BudgetPeriod withCategory(GoalBudgetEntry entry) throws ModelException {
		if (categories.containsKey(entry.name())) {
			throw ModelException.forExisting(entry);
		}
		var retval = clone();
		retval.categories.put(entry.name(), new GoalCategory(entry));
		return retval;
	}

	BudgetPeriod withCategory(IncomeBudgetEntry entry) throws ModelException {
		if (categories.containsKey(entry.name())) {
			throw ModelException.forExisting(entry);
		}
		var retval = clone();
		retval.categories.put(entry.name(), new IncomeCategory(entry));
		return retval;
	}

	BudgetPeriod withCategory(ManualGoalBudgetEntry entry) throws ModelException {
		if (categories.containsKey(entry.name())) {
			throw ModelException.forExisting(entry);
		}
		var retval = clone();
		retval.categories.put(entry.name(), new ManualGoalCategory(entry));
		return retval;
	}

	BudgetPeriod withCategory(ReserveBudgetEntry entry) throws ModelException {
		if (categories.containsKey(entry.name())) {
			throw ModelException.forExisting(entry);
		}
		var retval = clone();
		retval.categories.put(entry.name(), new ReserveCategory(entry));
		return retval;
	}

	BudgetPeriod withTransaction(CategoryTransactionEntry entry) throws ModelException {
		if (!categories.containsKey(entry.category())) {
			throw ModelException.forUnknown(entry);
		}
		var retval = clone();
		var category = retval.categories.get(entry.category());
		var allocation = category.withTransaction(entry);
		retval.categories.put(entry.category(), allocation.category());
		for (var c : retval.categories.entrySet()) {
			c.setValue(c.getValue().withAllocation(allocation));
		}
		return retval;
	}

	@Override
	public BudgetPeriod clone() {
		var retval = (BudgetPeriod) super.clone();
		retval.categories = new TreeMap<>(retval.categories);
		retval.owners = new TreeMap<>(retval.owners);
		return retval;
	}

}
