package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;

class Statement extends ModelItem {

	private Optional<LocalDate> closingDate = Optional.empty();
	private final Amount startBalance;
	private List<TransactionParticle> transactions = new ArrayList<>();

	Statement(Amount startBalance) {
		this.startBalance = startBalance;
	}

	Amount balance() {
		return transactions.stream().map(particle -> particle.amount()).reduce(startBalance,
				(total, value) -> total.add(value));
	}

	Optional<LocalDate> closingDate() {
		return closingDate;
	}

	Statement withReconciliation(AccountBalance balance) throws LedgerModelException {
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

	Statement withReconciliationRemainder(Statement other) {
		var retval = new Statement(other.balance());
		retval.transactions = new ArrayList<>(transactions);
		retval.transactions.removeAll(other.transactions);
		return retval;
	}

	Statement withTransaction(TransactionParticle particle) {
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
