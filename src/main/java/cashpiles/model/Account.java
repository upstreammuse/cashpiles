package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountCommand;
import cashpiles.util.Lists;

class Account {

	List<TransactionParticle> looseTransactions = new ArrayList<>();
	final LocalDate startDate;
	final AccountCommand.Status status;
	final List<Statement> statements = new ArrayList<>();

	Account(LocalDate startDate, AccountCommand.Status status) {
		this.startDate = startDate;
		this.status = status;
	}

	Amount balance() {
		var balance = statements.isEmpty() ? new Amount() : Lists.lastOf(statements).balance();
		for (var particle : looseTransactions) {
			balance = balance.add(particle.amount);
		}
		return balance;
	}

}
