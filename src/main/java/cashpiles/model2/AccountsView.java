package cashpiles.model2;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountCommand;

public interface AccountsView {

	Amount balance(AccountCommand.Status status);

	Amount balance(AccountCommand.Status status, int index);

	int size(AccountCommand.Status status);

	String name(AccountCommand.Status status, int index);

}
