package cashpiles.ledger;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

public class GoalBudgetEntry extends BudgetEntry {

	private Amount amount;
	private DateRange dates;
	private String owner;
	private boolean repeat = false;

	public GoalBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public Amount amount() {
		return amount;
	}

	public DateRange dates() {
		return dates;
	}

	public String owner() {
		return owner;
	}

	public boolean repeat() {
		return repeat;
	}

	public GoalBudgetEntry withAmount(Amount amount) {
		var retval = clone();
		retval.amount = amount;
		return retval;
	}

	public GoalBudgetEntry withDates(DateRange dates) {
		var retval = clone();
		retval.dates = dates;
		return retval;
	}

	public GoalBudgetEntry withOwner(String owner) {
		var retval = clone();
		retval.owner = owner;
		return retval;
	}

	public GoalBudgetEntry withRepeat(boolean repeat) {
		var retval = clone();
		retval.repeat = repeat;
		return retval;
	}

	@Override
	public GoalBudgetEntry withName(String name) {
		return (GoalBudgetEntry) super.withName(name);
	}

	@Override
	public void process(ItemProcessor processor) throws LedgerException {
		processor.process(this);
	}

	@Override
	public GoalBudgetEntry clone() {
		return (GoalBudgetEntry) super.clone();
	}

}
