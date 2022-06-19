package cashpiles.model2;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

public interface CategoriesView {

	Amount activity();

	Amount allocation();

	Amount balance();

	DateRange dates();

}
