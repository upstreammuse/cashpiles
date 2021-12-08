package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;

class Statement {

	LocalDate closingDate;
	final Amount startBalance;
	final List<TransactionParticle> transactions = new ArrayList<>();

	Statement(Amount startBalance) {
		this.startBalance = startBalance;
	}

	Amount balance() {
		var balance = startBalance;
		for (var particle : transactions) {
			balance = balance.add(particle.amount);
		}
		return balance;
	}

}
