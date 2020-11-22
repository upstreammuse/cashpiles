package cashpiles.ledger;

public class IncomeBudgetEntry extends BudgetEntry {

	public String name;
	public String owner;

	public IncomeBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

}
