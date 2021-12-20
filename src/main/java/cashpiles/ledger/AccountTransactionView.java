package cashpiles.ledger;

import cashpiles.currency.Amount;

public interface AccountTransactionView extends TransactionView {

	Amount accountAmount();

	int deferral();

	Transaction.Status accountStatus();

}
