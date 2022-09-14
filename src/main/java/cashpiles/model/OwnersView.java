package cashpiles.model;

import java.util.List;

import cashpiles.currency.Amount;

public interface OwnersView {

	Amount get(String name);

	List<String> names();

	int size();

}
