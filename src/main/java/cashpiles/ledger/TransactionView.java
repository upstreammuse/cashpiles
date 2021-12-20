package cashpiles.ledger;

import java.time.LocalDate;

import cashpiles.currency.Amount;

public interface TransactionView {

	Amount amount();

	LocalDate date();

	String payee();

}
