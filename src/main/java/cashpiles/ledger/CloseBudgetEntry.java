package cashpiles.ledger;

public class CloseBudgetEntry extends BudgetEntry {

	public CloseBudgetEntry(String name, String fileName, int lineNumber, String comment) {
		super(name, fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
