package cashpiles.account;

import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;

@SuppressWarnings("serial")
class AccountException extends LedgerException {

	public static AccountException forTypeChange(AccountCommand account) {
		return new AccountException(account, "Cannot change type of account '" + account.account() + "'");
	}

	public static AccountException forAlreadyOpen(AccountCommand account) {
		return new AccountException(account, "Cannot open account '" + account.account() + "' that is already open");
	}

	public static AccountException forNonZeroClose(AccountCommand account) {
		return new AccountException(account, "Cannot close account '" + account.account() + "' with non-zero balance");
	}

	public static AccountException forAlreadyClosed(AccountCommand account) {
		return new AccountException(account, "Cannot close account '" + account.account() + "' that is already closed");
	}

	public static AccountException forClosed(AccountTransactionEntry entry) {
		return new AccountException(entry, "Cannot use closed account '" + entry.account() + "'");
	}

	public static AccountException forClosed(TrackingTransactionEntry entry) {
		var builder = new StringBuilder();
		entry.trackingAccount.ifPresentOrElse(trackingAccount -> {
			builder.append(trackingAccount);
		}, () -> {
			throw new IllegalArgumentException("Tried to create an AccountException with missing account name");
		});
		return new AccountException(entry, "Cannot use closed account '" + builder + "'");
	}

	public static AccountException forClosed(UnbalancedTransaction transaction) {
		return new AccountException(transaction, "Cannot use closed account '" + transaction.account + "'");
	}

	public static AccountException forUnknown(AccountBalance balance) {
		return new AccountException(balance, "Cannot use unknown account '" + balance.account() + "'");
	}

	public static AccountException forUnknown(AccountTransactionEntry entry) {
		return new AccountException(entry, "Cannot use unknown account '" + entry.account() + "'");
	}

	public static AccountException forUnknown(TrackingTransactionEntry entry) {
		var builder = new StringBuilder();
		entry.trackingAccount.ifPresentOrElse(trackingAccount -> {
			builder.append(trackingAccount);
		}, () -> {
			throw new IllegalArgumentException("Tried to create an AccountException with missing account name");
		});
		return new AccountException(entry, "Cannot use unknown account '" + builder + "'");
	}

	public static AccountException forUnknown(UnbalancedTransaction transaction) {
		return new AccountException(transaction, "Cannot use unknown account '" + transaction.account + "'");
	}

	private AccountException(LedgerItem item, String message) {
		super(item, message);
	}

}
