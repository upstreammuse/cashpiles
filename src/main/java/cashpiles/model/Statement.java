package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountTransactionView;

public class Statement extends ModelItem implements AccountTransactionsView {

	private Optional<LocalDate> closingDate = Optional.empty();
	private Map<AccountTransactionView, Integer> deferrals = new HashMap<>();
	private final Amount startBalance;
	private List<AccountTransactionView> transactions = new ArrayList<>();

	public Statement(Amount startBalance) {
		this.startBalance = startBalance;
	}

	public Amount balance() {
		return transactions.stream().map(transaction -> transaction.accountAmount()).reduce(startBalance,
				(total, value) -> total.add(value));
	}

	@Override
	public Optional<LocalDate> closingDate() {
		return closingDate;
	}

	@Override
	public AccountTransactionView get(int index) {
		return transactions.get(index);
	}

	@Override
	public int size() {
		return transactions.size();
	}

	public Statement withReconciliation(AccountBalance balance) throws LedgerModelException {
		var retval = clone();
		retval.closingDate = Optional.of(balance.date());
		retval.transactions.removeIf(x -> x.date().compareTo(balance.date()) > 0);
		retval.transactions.removeIf(x -> switch (x.status()) {
		case CLEARED -> false;
		case DISPUTED -> false;
		case PENDING -> true;
		});
		for (var it = retval.transactions.listIterator(); it.hasNext(); /* inside */) {
			var view = it.next();
			var remainingDeferrals = retval.deferrals.get(view);
			if (remainingDeferrals != 0) {
				retval.deferrals.put(view, remainingDeferrals - 1);
				it.remove();
			}
		}
		if (!retval.balance().equals(balance.amount())) {
			throw LedgerModelException.forUnbalanced(balance, retval.balance());
		}
		return retval;
	}

	public Statement withReconciliationRemainder(Statement other) {
		var retval = new Statement(other.balance());
		retval.deferrals = new HashMap<>(deferrals);
		retval.transactions = new ArrayList<>(transactions);

		// TODO this could be consolidated with the code in withReconciliation
		var toRemove = new ArrayList<>(retval.transactions);
		toRemove.removeIf(x -> x.date().compareTo(other.closingDate.get()) > 0);
		toRemove.removeIf(x -> switch (x.status()) {
		case CLEARED -> false;
		case DISPUTED -> false;
		case PENDING -> true;
		});
		for (var it = toRemove.listIterator(); it.hasNext(); /* inside */) {
			var view = it.next();
			var remainingDeferrals = retval.deferrals.get(view);
			if (remainingDeferrals != 0) {
				retval.deferrals.put(view, remainingDeferrals - 1);
				it.remove();
			}
		}

		retval.transactions.removeAll(toRemove);
		return retval;
	}

	public Statement withTransaction(AccountTransactionView transaction) {
		var retval = clone();
		retval.transactions.add(transaction);
		retval.deferrals.put(transaction, transaction.deferral());
		return retval;
	}

	@Override
	public Statement clone() {
		var retval = (Statement) super.clone();
		retval.deferrals = new HashMap<>(deferrals);
		retval.transactions = new ArrayList<>(transactions);
		return retval;
	}

}
