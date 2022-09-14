package cashpiles.model;

import cashpiles.ledger.ManualGoalBudgetEntry;

//this is an immutable data class
class ManualGoalCategory extends Category {

	ManualGoalCategory(ManualGoalBudgetEntry entry) {
		super(entry.owner());
	}

	@Override
	public String type() {
		return "Manual";
	}

}
