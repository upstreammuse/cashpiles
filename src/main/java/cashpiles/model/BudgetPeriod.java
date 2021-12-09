package cashpiles.model;

import java.util.ArrayList;
import java.util.List;

import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.Transaction;
import cashpiles.time.DateRange;

class BudgetPeriod extends ModelItem {

	private final DateRange dates;
	private List<TransactionParticle> transactions = new ArrayList<>();

	BudgetPeriod(DateRange dates) {
		this.dates = dates;
	}

	DateRange dates() {
		return dates;
	}

	BudgetPeriod next() {
		return new BudgetPeriod(dates.next());
	}

	BudgetPeriod withTransaction(CategoryTransactionEntry entry) throws LedgerModelException {
		if (!dates.contains(entry.parent().date())) {
			throw LedgerModelException.forDateOutOfRange(entry, dates);
		}
		var retval = clone();
		retval.transactions.add(new TransactionParticle().withAmount(entry.amount()).withDate(entry.parent().date())
				.withPayee(entry.parent().payee()).withStatus(Transaction.Status.CLEARED));
		return retval;
	}

	@Override
	public BudgetPeriod clone() {
		var retval = (BudgetPeriod) super.clone();
		retval.transactions = new ArrayList<>(transactions);
		return retval;
	}

}
