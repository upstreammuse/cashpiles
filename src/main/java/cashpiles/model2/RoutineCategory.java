package cashpiles.model2;

import cashpiles.ledger.RoutineBudgetEntry;

//this is an immutable data class
class RoutineCategory extends Category {

	public RoutineCategory(RoutineBudgetEntry entry) {
		super(entry.owner());
	}

}
