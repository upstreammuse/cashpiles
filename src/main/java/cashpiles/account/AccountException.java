package cashpiles.account;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;

@SuppressWarnings("serial")
public class AccountException extends LedgerException {

	static AccountException forAccountWithOffBudget(AccountTransactionEntry entry) {
		return new AccountException(entry, "Cannot use off-budget account " + entry.account() + " for account entries");
	}

	static AccountException forAlreadyClosed(AccountCommand account) {
		return new AccountException(account, "Cannot close account '" + account.account() + "' that is already closed");
	}

	static AccountException forAlreadyOpen(AccountCommand account) {
		return new AccountException(account, "Cannot open account '" + account.account() + "' that is already open");
	}

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

	static AccountException forNonZeroClose(AccountCommand account) {
		return new AccountException(account, "Cannot close account '" + account.account() + "' with non-zero balance");
	}

	static AccountException forTrackingWithBudget(TrackingTransactionEntry entry) {
		var builder = new StringBuilder();
		entry.trackingAccount().ifPresentOrElse(trackingAccount -> builder.append(trackingAccount), () -> {
			throw new IllegalArgumentException("Tried to create a TransactionException with a missing account name");
		});
		return new AccountException(entry,
				"Cannot use on-budget account " + builder + " for category/tracking entries");
	}

	static AccountException forTypeChange(AccountCommand account) {
		return new AccountException(account, "Cannot change type of account '" + account.account() + "'");
	}

	static AccountException forUnbalanced(AccountBalance balance, Amount calculated) {
		return new AccountException(balance,
				"Statement balance " + balance.amount() + " does not match calculated balance " + calculated);
	}

	static AccountException forUnbalancedTransactionWithOnBudgetAccount(UnbalancedTransaction transaction) {
		return new AccountException(transaction,
				"Cannot use single-line transaction with on-budget account '" + transaction.account() + "'");
	}

	static AccountException forUnknown(AccountBalance balance) {
		return new AccountException(balance, "Cannot use unknown account '" + balance.account() + "'");
	}

	static AccountException forUnknown(AccountTransactionEntry entry) {
		return new AccountException(entry, "Cannot use unknown account '" + entry.account() + "'");
	}

	static AccountException forUnknown(TrackingTransactionEntry entry) {
		var builder = new StringBuilder();
		entry.trackingAccount().ifPresentOrElse(trackingAccount -> builder.append(trackingAccount), () -> {
			throw new IllegalArgumentException("Tried to create an AccountException with missing account name");
		});
		return new AccountException(entry, "Cannot use unknown account '" + builder + "'");
	}

	static AccountException forUnknown(UnbalancedTransaction transaction) {
		return new AccountException(transaction, "Cannot use unknown account '" + transaction.account() + "'");
	}

	protected AccountException(LedgerItem item, String message) {
		super(item, message);
	}

}
