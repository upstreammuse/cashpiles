package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;

public class Statement extends ModelItem {

	private Optional<LocalDate> closingDate = Optional.empty();
	private final Amount startBalance;
	private List<TransactionParticle> transactions = new ArrayList<>();

	public Statement(Amount startBalance) {
		this.startBalance = startBalance;
	}

	public Amount balance() {
		return transactions.stream().map(particle -> particle.amount()).reduce(startBalance,
				(total, value) -> total.add(value));
	}

	public Optional<LocalDate> closingDate() {
		return closingDate;
	}

	public TransactionParticle get(int index) {
		return transactions.get(index);
	}

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
		if (!retval.balance().equals(balance.amount())) {
			throw LedgerModelException.forUnbalanced(balance, retval.balance());
		}
		return retval;
	}

	public Statement withReconciliationRemainder(Statement other) {
		var retval = new Statement(other.balance());
		retval.transactions = new ArrayList<>(transactions);
		retval.transactions.removeAll(other.transactions);
		return retval;
	}

	public Statement withTransaction(TransactionParticle particle) {
		var retval = clone();
		retval.transactions.add(particle);
		return retval;
	}

	@Override
	public Statement clone() {
		var retval = (Statement) super.clone();
		retval.transactions = new ArrayList<>(transactions);
		return retval;
	}

}
