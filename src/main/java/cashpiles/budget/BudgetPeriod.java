package cashpiles.budget;

import java.util.HashMap;
import java.util.Map;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

public class BudgetPeriod {

	private Map<String, BudgetCategory> categories = new HashMap<>();
	private DateRange dates;
	private BudgetPeriod nextPeriod = null;

	public BudgetPeriod(DateRange dates) {
		this.dates = dates;
	}

	public Amount activity() {
		// TODO learn the stream version of this
		var activity = new Amount();
		for (var cat : categories.entrySet()) {
			activity = activity.add(cat.getValue().getActivity());
		}
		return activity;
	}

	public Amount allocation() {
		// TODO learn the stream version of this
		var allocation = new Amount();
		for (var cat : categories.entrySet()) {
			allocation = allocation.add(cat.getValue().getAllocation());
		}
		return allocation;
	}

	public Amount balance() {
		// TODO learn the stream version of this
		var balance = new Amount();
		for (var cat : categories.entrySet()) {
			balance = balance.add(cat.getValue().getBalance());
		}
		return balance;
	}

	public DateRange dates() {
		return dates;
	}

	public BudgetPeriod next() {
		if (nextPeriod == null) {
			nextPeriod = new BudgetPeriod(dates.next());
			// TODO learn the stream version of this
			for (var cat : categories.entrySet()) {
				if (cat.getValue().isActive()) {
					nextPeriod.categories.put(cat.getKey(), cat.getValue().clone());
				}
			}
		}
		return nextPeriod;
	}

}
