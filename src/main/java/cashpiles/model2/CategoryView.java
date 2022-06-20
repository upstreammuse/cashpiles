package cashpiles.model2;

import cashpiles.currency.Amount;

public interface CategoryView {

	Amount activity();

	Amount allocation();

	Amount balance();

	String owner();

	String type();

}
