package cashpiles.ledger;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

public class GoalBudgetEntry extends BudgetEntry {

	public Amount amount;
	public DateRange dates;
	public String name;
	public String owner;
	public boolean repeat;

	public GoalBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
