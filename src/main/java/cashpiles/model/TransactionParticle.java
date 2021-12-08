package cashpiles.model;

import java.time.LocalDate;

import cashpiles.currency.Amount;

class TransactionParticle extends ModelItem {

	private Amount amount;
	private LocalDate date;

	Amount amount() {
		return amount;
	}

	LocalDate date() {
		return date;
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

	@Override
	public TransactionParticle clone() {
		return (TransactionParticle) super.clone();
	}

}
