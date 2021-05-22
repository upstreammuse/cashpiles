package cashpiles.budget;

import java.time.DateTimeException;
import java.time.LocalDate;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;
import java.util.function.Supplier;

import javax.swing.JFrame;

import cashpiles.currency.Amount;
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
import cashpiles.ui.Windowable;
import cashpiles.ui.Windower;
import cashpiles.util.Comparisons;

public class BudgetPeriod implements Windowable {

	public Map<String, BudgetCategory> categories = new TreeMap<>();
	public Map<String, Amount> owners = new HashMap<>();
	private DateRange dates;
	private LocalDate lastTransactionDate;
	private BudgetPeriod nextPeriod = null;

	public BudgetPeriod(DateRange dates) {
		this.dates = dates;
		lastTransactionDate = dates.startDate();
	}

	public Amount activity() {
		var activity = new Amount();
		for (var cat : categories.entrySet()) {
			activity = activity.add(cat.getValue().getActivity());
		}
		return activity;
	}

	public void addTransaction(CategoryTransactionEntry entry) {
		if (!dates.contains(entry.parent.date)) {
			throw new DateTimeException("Cannot assign a transaction to a period that does not include its date");
		}
		if (!categories.containsKey(entry.category)) {
			throw new RuntimeException("Cannot use unknown category");
		}
		categories.get(entry.category).addTransaction(entry);
		lastTransactionDate = Comparisons.max(lastTransactionDate, entry.parent.date);
	}

	public void addTransaction(OwnerTransactionEntry entry) {
		if (!dates.contains(entry.parent.date)) {
			throw new DateTimeException("Cannot assign a transaction to a period that does not include its date");
		}
		if (!owners.containsKey(entry.owner)) {
			throw new RuntimeException("Cannot use unknown owner " + entry.owner);
		}
		owners.put(entry.owner, owners.get(entry.owner).add(entry.amount));
		lastTransactionDate = Comparisons.max(lastTransactionDate, entry.parent.date);
	}

	public Amount allocation() {
		var allocation = new Amount();
		for (var cat : categories.entrySet()) {
			allocation = allocation.add(cat.getValue().getAllocation());
		}
		return allocation;
	}

	public Amount balance() {
		var balance = new Amount();
		for (var cat : categories.entrySet()) {
			balance = balance.add(cat.getValue().getBalance());
		}
		for (var owner : owners.entrySet()) {
			balance = balance.add(owner.getValue());
		}
		return balance;
	}

	// TODO replace all the REs with something typed so that we can have decent
	// errors in the GUI
	public void configureCategory(CloseBudgetEntry entry) throws BudgetReconfigureException {
		if (!categories.containsKey(entry.category)) {
			throw BudgetReconfigureException.forClosedCategory(entry.category);
		}
		var closed = categories.get(entry.category);
		closed.close();
		for (var cat : categories.entrySet()) {
			closed.unlink(cat.getValue());
		}
		categories.remove(entry.category);
	}

	private void configureCategory(String name, Supplier<BudgetCategory> supplier) {
		if (categories.containsKey(name)) {
			throw new RuntimeException("Cannot create category that already exists");
		}
		var category = supplier.get();
		categories.put(name, category);
		if (!owners.containsKey(category.owner)) {
			owners.put(category.owner, new Amount());
		}
	}

	public void configureCategory(GoalBudgetEntry entry) {
		configureCategory(entry.name, () -> new GoalCategory(entry.name, entry));
	}

	public void configureCategory(IncomeBudgetEntry entry) {
		configureCategory(entry.name, () -> {
			var income = new IncomeCategory(entry.name, entry.owner);
			for (var cat : categories.entrySet()) {
				income.link(cat.getValue());
			}
			return income;
		});
	}

	public void configureCategory(ManualGoalBudgetEntry entry) {
		configureCategory(entry.name, () -> new ManualGoalCategory(entry.name, entry));
	}

	public void configureCategory(ReserveBudgetEntry entry) {
		configureCategory(entry.name, () -> {
			var reserve = new ReserveCategory(entry.name, entry.owner, entry.percentage);
			for (var cat : categories.entrySet()) {
				reserve.link(cat.getValue());
			}
			return reserve;
		});
	}

	public void configureCategory(RoutineBudgetEntry entry) {
		configureCategory(entry.name, () -> new RoutineCategory(entry.name, entry));
	}

	public void configureCategory(WithholdingBudgetEntry entry) {
		configureCategory(entry.name, () -> new WithholdingCategory(entry.name, entry));
	}

	public DateRange dates() {
		return dates;
	}

	@Override
	public JFrame makeWindow(Windower windower) {
		return windower.makeWindow(this);
	}

	public BudgetPeriod next() {
		if (nextPeriod == null) {
			nextPeriod = new BudgetPeriod(dates.next());
			categories.forEach((catName, cat) -> {
				owners.put(cat.owner, owners.get(cat.owner).add(cat.getAllocation().negate()));
				var nextCat = cat.next(nextPeriod.dates);
				nextPeriod.categories.forEach((otherCatName, otherCat) -> {
					nextCat.link(otherCat);
				});
				nextPeriod.categories.put(catName, nextCat);
			});
			nextPeriod.owners = new HashMap<>(owners);
		}
		return nextPeriod;
	}

	public void setDates(DateRange dateRange) throws BudgetReconfigureException {
		if (!dateRange.contains(lastTransactionDate)) {
			throw BudgetReconfigureException.forDateRange(dateRange);
		}
		dates = dateRange;
	}

}
