package cashpiles.ledger;

public class ManualGoalBudgetEntry extends BudgetEntry {

	public String name;
	public String owner;

	public ManualGoalBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
