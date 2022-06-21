package cashpiles.model2;

import cashpiles.currency.Amount;

public interface CategoryView {

	Amount activity();

	Amount allocation();

	Amount balance();

	Amount lifetimeAllocation();

	String owner();

	String type();

}
