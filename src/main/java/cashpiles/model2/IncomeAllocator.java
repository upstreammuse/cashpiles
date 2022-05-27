package cashpiles.model2;

import cashpiles.currency.Amount;

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
