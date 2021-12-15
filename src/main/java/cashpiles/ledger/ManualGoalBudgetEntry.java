package cashpiles.ledger;

public class ManualGoalBudgetEntry extends BudgetEntry {

	private String owner;

	public ManualGoalBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String owner() {
		return owner;
	}

	public ManualGoalBudgetEntry withOwner(String owner) {
		var retval = clone();
		retval.owner = owner;
		return retval;
	}

	@Override
	public ManualGoalBudgetEntry withName(String name) {
		return (ManualGoalBudgetEntry) super.withName(name);
	}

	@Override
	public void process(ItemProcessor processor) throws LedgerException {
		processor.process(this);
	}

	@Override
	public ManualGoalBudgetEntry clone() {
		return (ManualGoalBudgetEntry) super.clone();
	}

}
