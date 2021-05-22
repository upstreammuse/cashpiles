package cashpiles.ledger;

public class ManualGoalBudgetEntry extends BudgetEntry {

	public String owner;

	public ManualGoalBudgetEntry(String name, String fileName, int lineNumber, String comment) {
		super(name, fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
