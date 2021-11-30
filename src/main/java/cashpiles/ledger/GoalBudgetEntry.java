package cashpiles.ledger;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

public class GoalBudgetEntry extends BudgetEntry<GoalBudgetEntry> {

	private Amount amount;
	private DateRange dates;
	private String owner;
	private boolean repeat = false;

	public GoalBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public GoalBudgetEntry(GoalBudgetEntry other) {
		super(other);
		amount = other.amount;
		dates = other.dates;
		owner = other.owner;
		repeat = other.repeat;
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
		var retval = new GoalBudgetEntry(this);
		retval.amount = amount;
		return retval;
	}

	public GoalBudgetEntry withDates(DateRange dates) {
		var retval = new GoalBudgetEntry(this);
		retval.dates = dates;
		return retval;
	}

	public GoalBudgetEntry withOwner(String owner) {
		var retval = new GoalBudgetEntry(this);
		retval.owner = owner;
		return retval;
	}

	public GoalBudgetEntry withRepeat(boolean repeat) {
		var retval = new GoalBudgetEntry(this);
		retval.repeat = repeat;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

	@Override
	GoalBudgetEntry asChild() {
		return this;
	}

	@Override
	GoalBudgetEntry duplicate() {
		return new GoalBudgetEntry(this);
	}

}
