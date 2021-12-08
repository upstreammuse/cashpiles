package cashpiles.model;

import java.time.LocalDate;

import cashpiles.currency.Amount;
import cashpiles.ledger.Transaction;

class TransactionParticle extends ModelItem {

	private Amount amount;
	private LocalDate date;
	private Transaction.Status status;

	Amount amount() {
		return amount;
	}

	LocalDate date() {
		return date;
	}

	Transaction.Status status() {
		return status;
	}

	TransactionParticle withAmount(Amount amount) {
		var retval = clone();
		retval.amount = amount;
		return retval;
	}

	TransactionParticle withDate(LocalDate date) {
		var retval = clone();
		retval.date = date;
		return retval;
	}

	TransactionParticle withStatus(Transaction.Status status) {
		var retval = clone();
		retval.status = status;
		return retval;
	}

	@Override
	public TransactionParticle clone() {
		return (TransactionParticle) super.clone();
	}

	@Override
	public boolean equals(Object other) {
		if (other == null || !getClass().equals(other.getClass())) {
			return false;
		} else {
			var rhs = (TransactionParticle) other;
			return rhs.amount.equals(amount) && rhs.date.equals(date) && rhs.status.equals(status);
		}
	}

}
