package cashpiles.model;

import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.TransactionView;
import cashpiles.time.DateRange;

class BudgetPeriod extends ModelItem implements PeriodView {

	private Amount allocation = new Amount();
	private final DateRange dates;
	private List<TransactionView> entries = new ArrayList<>();
	private final Amount startBalance;

	BudgetPeriod(DateRange dates, Amount startBalance) {
		this.dates = dates;
		this.startBalance = startBalance;
	}

	@Override
	public Amount activity() {
		return entries.parallelStream().map(entry -> entry.amount()).reduce(new Amount(),
				(total, amount) -> total.add(amount));
	}

	@Override
	public Amount allocation() {
		return allocation;
	}

	@Override
	public Amount balance() {
		return startBalance.add(allocation()).add(activity());
	}

	@Override
	public DateRange dates() {
		return dates;
	}

	BudgetPeriod next() {
		return new BudgetPeriod(dates.next(), balance());
	}

	BudgetPeriod withAllocation(Amount allocation) {
		var retval = clone();
		retval.allocation = allocation;
		return retval;
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
