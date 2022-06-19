package cashpiles.model2;

import cashpiles.ledger.RoutineBudgetEntry;

class RoutineCategory extends Category {

	public RoutineCategory(RoutineBudgetEntry entry) {
		super(entry.owner());
	}

}
