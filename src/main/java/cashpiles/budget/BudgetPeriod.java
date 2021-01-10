package cashpiles.budget;

import java.time.DateTimeException;
import java.util.HashMap;
import java.util.Map;
import java.util.function.Supplier;

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

public class BudgetPeriod {

	private Map<String, BudgetCategory> categories = new HashMap<>();
	private Map<String, Amount> owners = new HashMap<>();
	private DateRange dates;
	private BudgetPeriod nextPeriod = null;

	public BudgetPeriod(DateRange dates) {
		this.dates = dates;
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
	}

	public void addTransaction(OwnerTransactionEntry entry) {
		if (!dates.contains(entry.parent.date)) {
			throw new DateTimeException("Cannot assign a transaction to a period that does not include its date");
		}
		if (!owners.containsKey(entry.owner)) {
			throw new RuntimeException("Cannot use unknown owner " + entry.owner);
		}
		owners.put(entry.owner, owners.get(entry.owner).add(entry.amount));
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

	public void configureCategory(CloseBudgetEntry entry) {
		if (!categories.containsKey(entry.category)) {
			// TODO replace all the REs with something typed so that we can have decent
			// errors in the GUI
			throw new RuntimeException("Cannot close entry that isn't open");
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
		categories.put(name, supplier.get());
	}

	public void configureCategory(GoalBudgetEntry entry) {
		configureCategory(entry.name, () -> new GoalCategory(entry.name, entry, owners));
	}

	public void configureCategory(IncomeBudgetEntry entry) {
		configureCategory(entry.name, () -> {
			var income = new IncomeCategory(entry.name, owners, entry.owner);
			for (var cat : categories.entrySet()) {
				income.link(cat.getValue());
			}
			return income;
		});
	}

	public void configureCategory(ManualGoalBudgetEntry entry) {
		configureCategory(entry.name, () -> new ManualGoalCategory(entry.name, entry, owners));
	}

	public void configureCategory(ReserveBudgetEntry entry) {
		configureCategory(entry.name, () -> {
			var reserve = new ReserveCategory(entry.name, owners, entry.owner, entry.percentage);
			for (var cat : categories.entrySet()) {
				reserve.link(cat.getValue());
			}
			return reserve;
		});
	}

	public void configureCategory(RoutineBudgetEntry entry) {
		configureCategory(entry.name, () -> new RoutineCategory(entry.name, entry, owners));
	}

	public void configureCategory(WithholdingBudgetEntry entry) {
		configureCategory(entry.name, () -> new WithholdingCategory(entry.name, entry, owners));
	}

	public DateRange dates() {
		return dates;
	}

	public BudgetPeriod next() {
		if (nextPeriod == null) {
			nextPeriod = new BudgetPeriod(dates.next());
			for (var cat : categories.entrySet()) {
				// TODO this isn't really cloning, so maybe this is a 'next()' method just like
				// this one
				nextPeriod.categories.put(cat.getKey(), cat.getValue().clone());
			}
			nextPeriod.owners = new HashMap<>(owners);
		}
		return nextPeriod;
	}

	public void setDates(DateRange dateRange) throws BudgetReconfigureException {
		for (var cat : categories.entrySet()) {
			if (cat.getValue().exceedsDates(dateRange)) {
				throw new BudgetReconfigureException(dateRange);
			}
		}
		dates = dateRange;
	}

}
