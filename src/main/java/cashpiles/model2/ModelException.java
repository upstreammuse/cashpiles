package cashpiles.model2;

import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;

@SuppressWarnings("serial")
class ModelException extends LedgerException {

	static ModelException forNoBudget(LedgerItem item) {
		return new ModelException(item, "Cannot use budget features without a budget configured");
	}

	static ModelException forOrphanBudgetEntry(CloseBudgetEntry entry) {
		return new ModelException(entry, "Cannot have partial budget entry");
	}

	static ModelException forOutOfSyncBudget(Budget budget) {
		return new ModelException(budget, "Cannot start new budget period here");
	}

	static ModelException forUnknownCategory(CategoryTransactionEntry entry) {
		return new ModelException(entry, "Cannot use unknown budget category");
	}

	static ModelException forUnknownCategory(CloseBudgetEntry entry) {
		return new ModelException(entry, "Cannot use unknown budget category");
	}

	ModelException(LedgerItem item, String message) {
		super(item, message);
	}

}
