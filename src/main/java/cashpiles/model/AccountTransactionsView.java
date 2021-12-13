package cashpiles.model;

import java.time.LocalDate;
import java.util.Optional;

import cashpiles.ledger.AccountTransactionView;

public interface AccountTransactionsView {

	Optional<LocalDate> closingDate();

	AccountTransactionView get(int index);

	int size();

}
