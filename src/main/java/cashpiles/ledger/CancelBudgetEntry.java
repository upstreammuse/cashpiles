package cashpiles.ledger;

public class CancelBudgetEntry extends BudgetEntry {

	public String category;
	public String goal;

	public CancelBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

}
