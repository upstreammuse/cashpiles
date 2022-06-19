package cashpiles.model2;

import cashpiles.ledger.ManualGoalBudgetEntry;

class ManualGoalCategory extends Category {

	ManualGoalCategory(ManualGoalBudgetEntry entry) {
		super(entry.owner());
	}

}
