package cashpiles.model;

import cashpiles.currency.Amount;

public interface CategoryView {

	Amount balance();

	PeriodView get(int index);

	String owner();

	int size();

	String type();

}
