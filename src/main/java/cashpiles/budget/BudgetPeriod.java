package cashpiles.budget;

import java.time.DateTimeException;
import java.util.HashMap;
import java.util.Map;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.ledger.OwnerTransactionEntry;
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
			throw new RuntimeException("Cannot use unknown owner");
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
		categories.get(entry.category).close();
		categories.remove(entry.category);
	}

	public void configureCategory(GoalBudgetEntry entry) {
		if (categories.containsKey(entry.name)) {
			throw new RuntimeException("Cannot create category that already exists");
		}
		categories.put(entry.name, new GoalsCategory(entry, owners));
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
