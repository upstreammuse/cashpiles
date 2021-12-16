package cashpiles.model;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

public interface PeriodView {

	Amount activity();

	Amount allocation();

	Amount balance();

	DateRange dates();

}
