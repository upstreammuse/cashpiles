package cashpiles.account;

import cashpiles.ledger.Account;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;

@SuppressWarnings("serial")
class AccountException extends LedgerException {

	public static AccountException forTypeChange(Account account) {
		return new AccountException(account, "Cannot change type of account '" + account.name + "'");
	}

	public static AccountException forAlreadyOpen(Account account) {
		return new AccountException(account, "Cannot open account '" + account.name + "' that is already open");
	}

	public static AccountException forNonZeroClose(Account account) {
		return new AccountException(account, "Cannot close account '" + account.name + "' with non-zero balance");
	}

	public static AccountException forAlreadyClosed(Account account) {
		return new AccountException(account, "Cannot close account '" + account.name + "' that is already closed");
	}

	public static AccountException forClosed(AccountTransactionEntry entry) {
		return new AccountException(entry, "Cannot use closed account '" + entry.account + "'");
	}

	public static AccountException forUnknown(AccountTransactionEntry entry) {
		return new AccountException(entry, "Cannot use unknown account '" + entry.account + "'");
	}

	private AccountException(LedgerItem item, String message) {
		super(item, message);
	}

}
