package cashpiles.model2;

import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.TreeMap;
import java.util.stream.Collectors;

import cashpiles.currency.Amount;
import cashpiles.ledger.Budget;
import cashpiles.ledger.BudgetEntry;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.ledger.IncomeBudgetEntry;
import cashpiles.ledger.ManualGoalBudgetEntry;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.ReserveBudgetEntry;
import cashpiles.ledger.RoutineBudgetEntry;
import cashpiles.ledger.WithholdingBudgetEntry;
import cashpiles.time.DateRange;

// this is an immutable data class
class BudgetPeriod extends ModelItem implements PeriodView {

	@SuppressWarnings("serial")
	private class CategoryMap extends TreeMap<String, Category> implements CategoriesView {

		CategoryMap() {
			super();
		}

		CategoryMap(CategoryMap other) {
			super(other);
		}

		@Override
		public CategoryView get(String name) {
			return super.get(name);
		}

		@Override
		public List<String> names() {
			return keySet().stream().collect(Collectors.toList());
		}

	}

	// the categories in this budget period
	private CategoryMap categories = new CategoryMap();

	// the dates covered by this period
	private DateRange dates;

	// the unallocated balances of each owner as of this budget period
	private Map<String, Amount> owners = new TreeMap<>();

	// create the first budget period
	BudgetPeriod(Budget budget) {
		dates = new DateRange(budget.date(), budget.period());
	}

	@Override
	public Amount activity() {
		var total = new Amount();
		for (var cat : categories.entrySet()) {
			total = total.add(cat.getValue().activity());
		}
		return total;
	}

	@Override
	public Amount allocation() {
		var total = new Amount();
		for (var cat : categories.entrySet()) {
			total = total.add(cat.getValue().allocation());
		}
		return total;
	}

	@Override
	public Amount balance() {
		var total = new Amount();
		for (var cat : categories.entrySet()) {
			total = total.add(cat.getValue().balance());
		}
		return total;
	}

	@Override
	public CategoriesView categories() {
		return categories;
	}

	@Override
	public BudgetPeriod clone() {
		var retval = (BudgetPeriod) super.clone();
		retval.categories = new CategoryMap(retval.categories);
		retval.owners = new TreeMap<>(retval.owners);
		return retval;
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
	@Override
	public DateRange dates() {
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
		var retval = withCategoryCommon(entry);
		retval.categories.put(entry.name(), new GoalCategory(entry));
		if (!retval.owners.containsKey(entry.owner())) {
			retval.owners.put(entry.owner(), new Amount());
		}
		return retval;
	}

	BudgetPeriod withCategory(IncomeBudgetEntry entry) throws ModelException {
		var retval = withCategoryCommon(entry);
		retval.categories.put(entry.name(), new IncomeCategory(entry));
		if (!retval.owners.containsKey(entry.owner())) {
			retval.owners.put(entry.owner(), new Amount());
		}
		return retval;
	}

	BudgetPeriod withCategory(ManualGoalBudgetEntry entry) throws ModelException {
		var retval = withCategoryCommon(entry);
		retval.categories.put(entry.name(), new ManualGoalCategory(entry));
		if (!retval.owners.containsKey(entry.owner())) {
			retval.owners.put(entry.owner(), new Amount());
		}
		return retval;
	}

	BudgetPeriod withCategory(ReserveBudgetEntry entry) throws ModelException {
		var retval = withCategoryCommon(entry);
		retval.categories.put(entry.name(), new ReserveCategory(entry));
		if (!retval.owners.containsKey(entry.owner())) {
			retval.owners.put(entry.owner(), new Amount());
		}
		return retval;
	}

	BudgetPeriod withCategory(RoutineBudgetEntry entry) throws ModelException {
		var retval = withCategoryCommon(entry);
		retval.categories.put(entry.name(), new RoutineCategory(entry));
		if (!retval.owners.containsKey(entry.owner())) {
			retval.owners.put(entry.owner(), new Amount());
		}
		return retval;
	}

	BudgetPeriod withCategory(WithholdingBudgetEntry entry) throws ModelException {
		var retval = withCategoryCommon(entry);
		retval.categories.put(entry.name(), new WithholdingCategory(entry));
		if (!retval.owners.containsKey(entry.owner())) {
			retval.owners.put(entry.owner(), new Amount());
		}
		return retval;
	}

	private BudgetPeriod withCategoryCommon(BudgetEntry entry) throws ModelException {
		if (categories.containsKey(entry.name())) {
			throw ModelException.forExisting(entry);
		}
		return clone();
	}

	BudgetPeriod withTransaction(CategoryTransactionEntry entry) throws ModelException {
		if (!categories.containsKey(entry.category())) {
			throw ModelException.forUnknown(entry);
		}
		var retval = clone();
		var category = (Category) retval.categories.get(entry.category());
		var allocation = category.withTransaction(entry);
		retval.categories.put(entry.category(), allocation.category());
		for (var c : retval.categories.entrySet()) {
			c.setValue(c.getValue().withAllocation(allocation));
		}
		return retval;
	}

	BudgetPeriod withTransaction(OwnerTransactionEntry entry) throws ModelException {
		if (!owners.containsKey(entry.owner())) {
			throw ModelException.forUnknown(entry);
		}
		var retval = clone();
		retval.owners.put(entry.owner(), retval.owners.get(entry.owner()).add(entry.amount()));
		return retval;
	}

}
