package cashpiles.account;

import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;

@SuppressWarnings("serial")
public class AccountException extends LedgerException {

	static AccountException forClosed(AccountTransactionEntry entry) {
		return new AccountException(entry, "Cannot use closed account '" + entry.account() + "'");
	}

	static AccountException forClosed(TrackingTransactionEntry entry) {
		var builder = new StringBuilder();
		entry.trackingAccount().ifPresentOrElse(trackingAccount -> builder.append(trackingAccount), () -> {
			throw new IllegalArgumentException("Tried to create an AccountException with missing account name");
		});
		return new AccountException(entry, "Cannot use closed account '" + builder + "'");
	}

	static AccountException forClosed(UnbalancedTransaction transaction) {
		return new AccountException(transaction, "Cannot use closed account '" + transaction.account() + "'");
	}

	static AccountException forUnbalancedTransactionWithOnBudgetAccount(UnbalancedTransaction transaction) {
		return new AccountException(transaction,
				"Cannot use single-line transaction with on-budget account '" + transaction.account() + "'");
	}

	static AccountException forUnknown(AccountTransactionEntry entry) {
		return new AccountException(entry, "Cannot use unknown account '" + entry.account() + "'");
	}

	static AccountException forUnknown(UnbalancedTransaction transaction) {
		return new AccountException(transaction, "Cannot use unknown account '" + transaction.account() + "'");
	}

	protected AccountException(LedgerItem item, String message) {
		super(item, message);
	}

}
