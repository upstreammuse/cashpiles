package cashpiles.model;

import java.time.LocalDate;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;

@SuppressWarnings("serial")
public class LedgerModelException extends LedgerException {

	static LedgerModelException forAlreadyClosed(AccountCommand account) {
		return new LedgerModelException(account,
				"Cannot close account '" + account.account() + "' that is already closed.");
	}

	static LedgerModelException forAlreadyOpen(AccountCommand account) {
		return new LedgerModelException(account,
				"Cannot open account '" + account.account() + "' that is already open.");
	}

	static LedgerModelException forBudgetNeeded(AccountTransactionEntry entry) {
		return new LedgerModelException(entry,
				"Account '" + entry.account() + "' is not on-budget, which is needed for account entries.");
	}

	static LedgerModelException forNonZeroClose(AccountCommand account) {
		return new LedgerModelException(account,
				"Cannot close account '" + account.account() + "' with non-zero balance.");
	}

	static LedgerModelException forOffBudgetNeeded(TrackingTransactionEntry entry) {
		var builder = new StringBuilder();
		entry.trackingAccount().ifPresentOrElse(trackingAccount -> builder.append(trackingAccount), () -> {
			throw new IllegalArgumentException("Tried to create a LedgerModelException with a missing account name.");
		});
		return new LedgerModelException(entry,
				"Cannot use on-budget account '" + builder + "' for category/tracking entries.");
	}

	static LedgerModelException forOffBudgetNeeded(UnbalancedTransaction transaction) {
		return new LedgerModelException(transaction, "Account '" + transaction.account()
				+ "' is not off-budget, which is needed for unbalanced transactions.");
	}

	static LedgerModelException forTooEarly(AccountTransactionEntry entry, LocalDate startDate) {
		return new LedgerModelException(entry, "Transaction date " + entry.parent().date()
				+ " is before the account start date of " + startDate + ".");
	}

	static LedgerModelException forTooEarly(TrackingTransactionEntry entry, LocalDate startDate) {
		return new LedgerModelException(entry, "Transaction date " + entry.parent().date()
				+ " is before the account start date of " + startDate + ".");
	}

	static LedgerModelException forTooEarly(UnbalancedTransaction transaction, LocalDate startDate) {
		return new LedgerModelException(transaction,
				"Transaction date " + transaction.date() + " is before the account start date of " + startDate + ".");
	}

	static LedgerModelException forTypeChange(AccountCommand account) {
		return new LedgerModelException(account, "Cannot change type of account '" + account.account() + "'.");
	}

	static LedgerModelException forUnbalanced(AccountBalance balance, Amount calculated) {
		return new LedgerModelException(balance,
				"Statement balance " + balance.amount() + " and calculated balance " + calculated + " do not match.");
	}

	static LedgerModelException forUnknown(AccountBalance balance) {
		return new LedgerModelException(balance,
				"Cannot balance account '" + balance.account() + "' that is not open.");
	}

	static LedgerModelException forUnknown(AccountTransactionEntry entry) {
		return new LedgerModelException(entry,
				"Cannot use account '" + entry.account() + "' that is not open in a transaction.");
	}

	static LedgerModelException forUnknown(TrackingTransactionEntry entry) {
		var builder = new StringBuilder();
		entry.trackingAccount().ifPresentOrElse(trackingAccount -> builder.append(trackingAccount), () -> {
			throw new IllegalArgumentException("Tried to create a LedgerModelException with missing account name");
		});
		return new LedgerModelException(entry, "Cannot use unknown account '" + builder + "'.");
	}

	static LedgerModelException forUnknown(UnbalancedTransaction transaction) {
		return new LedgerModelException(transaction,
				"Cannot use account '" + transaction.account() + "' that is not open in a transaction.");
	}

	protected LedgerModelException(LedgerItem item, String message) {
		super(item, message);
	}

}
