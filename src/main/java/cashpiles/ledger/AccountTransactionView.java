package cashpiles.ledger;

import cashpiles.currency.Amount;
import cashpiles.model.TransactionView;

public interface AccountTransactionView extends TransactionView {

	Amount accountAmount();

	int deferral();

	Transaction.Status accountStatus();

}
