package cashpiles.model2;

import java.time.LocalDate;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.Budget;
import cashpiles.ledger.BudgetEntry;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.DatedLedgerItem;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.TransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;
import cashpiles.time.DateRange;

@SuppressWarnings("serial")
class ModelException extends LedgerException {

	static ModelException forAlreadyClosed(AccountCommand account) {
		return new ModelException(account, "Cannot close account '" + account.account() + "' that is already closed.");
	}

	static ModelException forAlreadyOpen(AccountCommand account) {
		return new ModelException(account, "Cannot open account '" + account.account() + "' that is already open.");
	}

	static ModelException forBudgetNeeded(AccountTransactionEntry entry) {
		return new ModelException(entry,
				"Account '" + entry.account() + "' is not on-budget, which is needed for account entries.");
	}

	static ModelException forBudgetStartDate(Budget budget, LocalDate startDate) {
		return new ModelException(budget, "Cannot start new budget period on " + budget.date()
				+ ".  Next available start date is " + startDate + ".");
	}

	static ModelException forExisting(BudgetEntry entry) {
		return new ModelException(entry, "Budget category already exists");
	}

	static ModelException forNoBudget(LedgerItem item) {
		return new ModelException(item, "Cannot use budget features without a budget configured");
	}

	static ModelException forNonZeroClose(AccountCommand account) {
		return new ModelException(account, "Cannot close account '" + account.account() + "' with non-zero balance.");
	}

	static ModelException forOffBudgetNeeded(TrackingTransactionEntry entry) {
		var builder = new StringBuilder();
		entry.trackingAccount().ifPresentOrElse(trackingAccount -> builder.append(trackingAccount), () -> {
			throw new IllegalArgumentException("Tried to create a LedgerModelException with a missing account name.");
		});
		return new ModelException(entry,
				"Cannot use on-budget account '" + builder + "' for category/tracking entries.");
	}

	static ModelException forOffBudgetNeeded(UnbalancedTransaction transaction) {
		return new ModelException(transaction, "Account '" + transaction.account()
				+ "' is not off-budget, which is needed for unbalanced transactions.");
	}

	static ModelException forOrphan(BudgetEntry entry) {
		return new ModelException(entry, "Cannot have partial budget entry");
	}

	static ModelException forOrphan(TransactionEntry entry) {
		return new ModelException(entry, "Cannot have partial transaction entry");
	}

	static ModelException forOutOfRange(DatedLedgerItem item, DateRange dates) {
		return new ModelException(item, "Item dated " + item.date() + " does not occur in current budget period dated "
				+ dates.startDate() + " to " + dates.endDate() + ".");
	}

	static ModelException forTooEarly(AccountTransactionEntry entry, LocalDate startDate) {
		return new ModelException(entry, "Transaction date " + entry.parent().date()
				+ " is before the account start date of " + startDate + ".");
	}

	static ModelException forTooEarly(TrackingTransactionEntry entry, LocalDate startDate) {
		return new ModelException(entry, "Transaction date " + entry.parent().date()
				+ " is before the account start date of " + startDate + ".");
	}

	static ModelException forTooEarly(UnbalancedTransaction transaction, LocalDate startDate) {
		return new ModelException(transaction,
				"Transaction date " + transaction.date() + " is before the account start date of " + startDate + ".");
	}

	static ModelException forTypeChange(AccountCommand account) {
		return new ModelException(account, "Cannot change type of account '" + account.account() + "'.");
	}

	static ModelException forUnbalanced(AccountBalance balance, Amount calculated) {
		return new ModelException(balance,
				"Statement balance " + balance.amount() + " and calculated balance " + calculated + " do not match.");
	}

	static ModelException forUncleared(AccountBalance balance) {
		return new ModelException(balance, "Account balance includes uncleared transactions.");
	}

	static ModelException forUnknown(AccountBalance balance) {
		return new ModelException(balance, "Cannot balance account '" + balance.account() + "' that is not open.");
	}

	static ModelException forUnknown(AccountTransactionEntry entry) {
		return new ModelException(entry,
				"Cannot use account '" + entry.account() + "' that is not open in a transaction.");
	}

	static ModelException forUnknown(CategoryTransactionEntry entry) {
		return new ModelException(entry, "Cannot use unknown category '" + entry.category() + "' in a transaction.");
	}

	static ModelException forUnknown(CloseBudgetEntry entry) {
		return new ModelException(entry, "Cannot use unknown budget category");
	}

	static ModelException forUnknown(OwnerTransactionEntry entry) {
		return new ModelException(entry, "Cannot use unknown budget category owner");
	}

	static ModelException forUnknown(TrackingTransactionEntry entry) {
		var builder = new StringBuilder();
		entry.trackingAccount().ifPresentOrElse(trackingAccount -> builder.append(trackingAccount), () -> {
			throw new IllegalArgumentException("Tried to create a LedgerModelException with missing account name");
		});
		return new ModelException(entry, "Cannot use unknown account '" + builder + "'.");
	}

	static ModelException forUnknown(UnbalancedTransaction transaction) {
		return new ModelException(transaction,
				"Cannot use account '" + transaction.account() + "' that is not open in a transaction.");
	}

	ModelException(LedgerItem item, String message) {
		super(item, message);
	}

}
