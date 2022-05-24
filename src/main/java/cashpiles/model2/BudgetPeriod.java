package cashpiles.model2;

import java.util.Map;
import java.util.TreeMap;

import cashpiles.currency.Amount;
import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.time.DateRange;

// this is an immutable data class
class BudgetPeriod extends ModelItem {

	// the categories in this budget period
	private Map<String, Category> categories = new TreeMap<>();

	// the dates covered by this period
	private DateRange dates;

	// create the first budget period
	BudgetPeriod(Budget budget) {
		dates = new DateRange(budget.date(), budget.period());
	}

	// remove the category if it has a zero balance, which is safe because it only
	// happens in the context of a budget, which can only happen at the beginning of
	// a new budget period
	BudgetPeriod close(CloseBudgetEntry entry) throws ModelException {
		var retval = clone();
		var category = retval.categories.get(entry.name());
		if (!category.balance().equals(new Amount())) {
			throw ModelException.forNonZeroBalance(entry);
		}
		retval.categories.remove(entry.name());
		return retval;
	}

	// get the dates of the period
	DateRange dates() {
		return dates;
	}

	// generate the next budget period based on the current one, which allows us to
	// continue the budget cycle without explicit budget entries in the file
	BudgetPeriod next() {
		var retval = clone();
		retval.dates = retval.dates.next();
		for (var entry : retval.categories.entrySet()) {
			entry.setValue(entry.getValue().reset());
		}
		return retval;
	}

	// generate the next budget period based on the current one, but use a different
	// duration for the new period
	BudgetPeriod next(Budget budget) throws ModelException {
		var retval = clone();
		retval.dates = new DateRange(retval.dates.next().startDate(), budget.period());
		if (!retval.dates.startDate().equals(budget.date())) {
			throw ModelException.forOutOfSyncBudget(budget);
		}
		for (var entry : retval.categories.entrySet()) {
			entry.setValue(entry.getValue().reset());
		}
		return retval;
	}

	BudgetPeriod withTransaction(CategoryTransactionEntry entry) throws ModelException {
		var retval = clone();
		if (!retval.categories.containsKey(entry.category())) {
			throw ModelException.forUnknownCategory(entry);
		}
		var category = retval.categories.get(entry.category());
		retval.categories.put(entry.category(), category.withTransaction(entry));
		return retval;
	}

	@Override
	public BudgetPeriod clone() {
		var retval = (BudgetPeriod) super.clone();
		retval.categories = new TreeMap<>(retval.categories);
		return retval;
	}

}
