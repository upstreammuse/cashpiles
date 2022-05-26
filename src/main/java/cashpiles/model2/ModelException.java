package cashpiles.model2;

import cashpiles.ledger.Budget;
import cashpiles.ledger.BudgetEntry;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.TransactionEntry;

@SuppressWarnings("serial")
class ModelException extends LedgerException {

	static ModelException forExisting(BudgetEntry entry) {
		return new ModelException(entry, "Budget category already exists");
	}

	static ModelException forNoBudget(LedgerItem item) {
		return new ModelException(item, "Cannot use budget features without a budget configured");
	}

	static ModelException forOrphan(BudgetEntry entry) {
		return new ModelException(entry, "Cannot have partial budget entry");
	}

	static ModelException forOrphan(TransactionEntry entry) {
		return new ModelException(entry, "Cannot have partial transaction entry");
	}

	static ModelException forOutOfSync(Budget budget) {
		return new ModelException(budget, "Cannot start new budget period here");
	}

	static ModelException forUnknown(CategoryTransactionEntry entry) {
		return new ModelException(entry, "Cannot use unknown budget category");
	}

	static ModelException forUnknown(CloseBudgetEntry entry) {
		return new ModelException(entry, "Cannot use unknown budget category");
	}

	ModelException(LedgerItem item, String message) {
		super(item, message);
	}

}
