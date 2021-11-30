package cashpiles.ledger;

public class ManualGoalBudgetEntry extends BudgetEntry<ManualGoalBudgetEntry> {

	private String owner;

	public ManualGoalBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public ManualGoalBudgetEntry(ManualGoalBudgetEntry other) {
		super(other);
		owner = other.owner;
	}

	public String owner() {
		return owner;
	}

	public ManualGoalBudgetEntry withOwner(String owner) {
		var retval = duplicate();
		retval.owner = owner;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

	@Override
	ManualGoalBudgetEntry asChild() {
		return this;
	}

	@Override
	ManualGoalBudgetEntry duplicate() {
		return new ManualGoalBudgetEntry(this);
	}

}
