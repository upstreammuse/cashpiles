package cashpiles.ledger;

public class GoalsBudgetEntry extends BudgetEntry {

	public String name;
	public String owner;

	public GoalsBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

}
