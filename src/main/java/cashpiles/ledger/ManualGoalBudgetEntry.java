package cashpiles.ledger;

public class ManualGoalBudgetEntry extends BudgetEntry {

	public String name;
	public String owner;

	public ManualGoalBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

}
