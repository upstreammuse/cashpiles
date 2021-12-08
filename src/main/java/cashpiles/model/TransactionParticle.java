package cashpiles.model;

import java.time.LocalDate;

import cashpiles.currency.Amount;
import cashpiles.ledger.Transaction;

public class TransactionParticle extends ModelItem {

	private Amount amount;
	private LocalDate date;
	private String payee;
	private Transaction.Status status;

	public Amount amount() {
		return amount;
	}

	public LocalDate date() {
		return date;
	}

	public String payee() {
		return payee;
	}

	public Transaction.Status status() {
		return status;
	}

	public TransactionParticle withAmount(Amount amount) {
		var retval = clone();
		retval.amount = amount;
		return retval;
	}

	public TransactionParticle withDate(LocalDate date) {
		var retval = clone();
		retval.date = date;
		return retval;
	}

	public TransactionParticle withPayee(String payee) {
		var retval = clone();
		retval.payee = payee;
		return retval;
	}

	public TransactionParticle withStatus(Transaction.Status status) {
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
