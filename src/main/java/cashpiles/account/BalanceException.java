package cashpiles.account;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;

@SuppressWarnings("serial")
public class BalanceException extends LedgerException {

	public static BalanceException forUnbalanced(AccountBalance balance, Amount calculated) {
		return new BalanceException(balance,
				"Statement balance " + balance.amount() + " does not match calculated balance " + calculated);
	}

	private BalanceException(LedgerItem item, String why) {
		super(item, why);
	}

}
