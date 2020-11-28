package cashpiles.budget;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

// TODO seems like the concept of startbalance + stuff -> endbalance is something that could be generalized into an interface that categories, budget periods, statements, etc. would all implement.  That could also be the place where the "update dependent calculations" concept could come in, but not exactly sure how that would play out 
interface BudgetCategory extends Cloneable {

	BudgetCategory clone();

	boolean exceedsDates(DateRange dateRange);

	Amount getActivity();

	Amount getAllocation();

	Amount getBalance();

	boolean isActive();

	void setActive(boolean active);

}
