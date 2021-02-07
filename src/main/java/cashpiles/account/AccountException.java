package cashpiles.account;

import cashpiles.ledger.Account;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;

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

	// TODO this will work but is technically wrong since it assumes the account is
	// set
	public static AccountException forClosed(TrackingTransactionEntry entry) {
		return new AccountException(entry, "Cannot use closed account '" + entry.trackingAccount.get() + "'");
	}

	public static AccountException forClosed(UnbalancedTransaction transaction) {
		return new AccountException(transaction, "Cannot use closed account '" + transaction.account + "'");
	}

	public static AccountException forUnknown(AccountBalance balance) {
		return new AccountException(balance, "Cannot use unknown account '" + balance.account + "'");
	}

	public static AccountException forUnknown(AccountTransactionEntry entry) {
		return new AccountException(entry, "Cannot use unknown account '" + entry.account + "'");
	}

	// TODO this will work but is technically wrong since it assumes the account is
	// set
	public static AccountException forUnknown(TrackingTransactionEntry entry) {
		return new AccountException(entry, "Cannot use unknown account '" + entry.trackingAccount.get() + "'");
	}

	public static AccountException forUnknown(UnbalancedTransaction transaction) {
		return new AccountException(transaction, "Cannot use unknown account '" + transaction.account + "'");
	}

	private AccountException(LedgerItem item, String message) {
		super(item, message);
	}

}
