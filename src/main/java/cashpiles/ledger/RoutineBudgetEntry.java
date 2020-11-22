package cashpiles.ledger;

public class RoutineBudgetEntry extends BudgetEntry {

	public String name;
	public String owner;

	public RoutineBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

}
