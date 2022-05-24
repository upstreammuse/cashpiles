package cashpiles.model2;

import cashpiles.ledger.Budget;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;

@SuppressWarnings("serial")
class ModelException extends LedgerException {

	static ModelException forNonZeroBalance(CloseBudgetEntry entry) {
		return new ModelException(entry, "Cannot close budget category with nonzero balance");
	}

	static ModelException forOrphanBudgetEntry(CloseBudgetEntry entry) {
		return new ModelException(entry, "Cannot have partial budget entry");
	}

	static ModelException forOutOfSyncBudget(Budget budget) {
		return new ModelException(budget, "Cannot start new budget period here");
	}

	ModelException(LedgerItem item, String message) {
		super(item, message);
	}

}
