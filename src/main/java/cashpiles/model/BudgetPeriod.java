package cashpiles.model;

import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.time.DateRange;

class BudgetPeriod extends ModelItem {

	private final DateRange dates;
	private List<CategoryTransactionEntry> entries = new ArrayList<>();
	private final Amount startBalance;

	BudgetPeriod(DateRange dates, Amount startBalance) {
		this.dates = dates;
		this.startBalance = startBalance;
	}

	Amount balance() {
		return startBalance.add(entries.parallelStream().map(entry -> entry.amount()).reduce(new Amount(),
				(total, amount) -> total.add(amount)));
	}

	DateRange dates() {
		return dates;
	}

	BudgetPeriod next() {
		return new BudgetPeriod(dates.next(), balance());
	}

	BudgetPeriod withTransaction(CategoryTransactionEntry entry) throws LedgerModelException {
		if (!dates.contains(entry.parent().date())) {
			throw LedgerModelException.forDateOutOfRange(entry, dates);
		}
		var retval = clone();
		retval.entries.add(entry);
		return retval;
	}

	@Override
	public BudgetPeriod clone() {
		var retval = (BudgetPeriod) super.clone();
		retval.entries = new ArrayList<>(entries);
		return retval;
	}

}
