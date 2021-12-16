package cashpiles.model;

import cashpiles.currency.Amount;

public interface CategoryView {

	Amount balance();

	String owner();

	String type();

}
