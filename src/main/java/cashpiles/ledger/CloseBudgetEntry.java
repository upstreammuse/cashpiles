package cashpiles.ledger;

public class CloseBudgetEntry extends BudgetEntry {

	public CloseBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	public CloseBudgetEntry withName(String name) {
		return (CloseBudgetEntry) super.withName(name);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
