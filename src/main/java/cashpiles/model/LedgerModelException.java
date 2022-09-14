package cashpiles.model;

import java.time.LocalDate;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.BudgetEntry;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.time.DateRange;

@SuppressWarnings("serial")
public class LedgerModelException extends LedgerException {

	static LedgerModelException forDateOutOfRange(CategoryTransactionEntry entry, DateRange dates) {
		return new LedgerModelException(entry, "Transaction dated " + entry.parent().date()
				+ " does not belong in budget period dated " + dates.startDate() + " to " + dates.endDate() + ".");
	}

	static LedgerModelException forExistingCategory(BudgetEntry entry) {
		return new LedgerModelException(entry, "Budget category '" + entry.name() + "' already in use.");
	}

	static LedgerModelException forTooEarly(CategoryTransactionEntry entry, LocalDate startDate) {
		return new LedgerModelException(entry, "Transaction date " + entry.parent().date()
				+ " is before the category start date of " + startDate + ".");
	}

	static LedgerModelException forUnbalanced(AccountBalance balance, Amount calculated) {
		return new LedgerModelException(balance,
				"Statement balance " + balance.amount() + " and calculated balance " + calculated + " do not match.");
	}

	static LedgerModelException forUncleared(AccountBalance balance) {
		return new LedgerModelException(balance, "Account balance includes uncleared transactions.");
	}

	static LedgerModelException forUnknown(CategoryTransactionEntry entry) {
		return new LedgerModelException(entry,
				"Cannot use unknown category '" + entry.category() + "' in a transaction.");
	}

	protected LedgerModelException(LedgerItem item, String message) {
		super(item, message);
	}

}
