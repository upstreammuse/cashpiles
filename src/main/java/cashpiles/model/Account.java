package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountCommand;
import cashpiles.util.Lists;

class Account {

	final LocalDate startDate;
	final AccountCommand.Status status;
	final List<Statement> statements = new ArrayList<>();

	Account(LocalDate startDate, AccountCommand.Status status) {
		this.startDate = startDate;
		this.status = status;
		statements.add(new Statement(new Amount()));
	}

	Amount balance() {
		return Lists.lastOf(statements).balance();
	}

}
