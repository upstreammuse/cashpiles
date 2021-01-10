package cashpiles.ledger;

public class CloseBudgetEntry extends BudgetEntry {

	public String category;

	public CloseBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
