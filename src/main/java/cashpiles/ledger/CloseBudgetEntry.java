package cashpiles.ledger;

public class CloseBudgetEntry extends BudgetEntry {

	public String category;

	public CloseBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

}
