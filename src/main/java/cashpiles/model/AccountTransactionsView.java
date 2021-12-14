package cashpiles.model;

import java.time.LocalDate;
import java.util.Optional;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountTransactionView;

public interface AccountTransactionsView {

	Amount balance();

	Optional<LocalDate> closingDate();

	AccountTransactionView get(int index);

	int size();

	Amount startingBalance();

}
