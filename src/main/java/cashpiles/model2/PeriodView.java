package cashpiles.model2;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

public interface PeriodView {

	Amount activity();

	Amount allocation();

	Amount balance();

	CategoriesView categories();

	DateRange dates();

	OwnersView owners();

}
