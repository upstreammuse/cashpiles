package cashpiles.ledger;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

// TODO this class and Goal are basically the same, except one repeats, and goals are linked to a parent category unlike this
public class ReserveAmountBudgetEntry extends BudgetEntry {

	public Amount amount;
	public DateRange dates;
	public String name;
	public String owner;

	public ReserveAmountBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

}
