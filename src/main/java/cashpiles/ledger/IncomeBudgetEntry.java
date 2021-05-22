package cashpiles.ledger;

public class IncomeBudgetEntry extends BudgetEntry {

	public String owner;

	public IncomeBudgetEntry(String name, String fileName, int lineNumber, String comment) {
		super(name, fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
