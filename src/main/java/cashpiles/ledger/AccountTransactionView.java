package cashpiles.ledger;

import java.time.LocalDate;

import cashpiles.currency.Amount;

public interface AccountTransactionView {

	Amount accountAmount();

	LocalDate date();

	int deferral();

	String payee();

	Transaction.Status status();

}