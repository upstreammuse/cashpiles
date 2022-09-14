package cashpiles.model;

import cashpiles.currency.Amount;

//this is an immutable data class
class IncomeAllocator extends CrossAllocator {

	private final Amount amount;

	IncomeAllocator(Category category, Amount amount) {
		super(category);
		this.amount = amount;
	}

	@Override
	ReserveCategory allocate(ReserveCategory category) {
		return category.withIncomeAllocation(amount);
	}

}
