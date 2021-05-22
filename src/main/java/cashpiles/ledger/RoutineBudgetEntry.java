package cashpiles.ledger;

public class RoutineBudgetEntry extends BudgetEntry {

	public String owner;

	public RoutineBudgetEntry(String name, String fileName, int lineNumber, String comment) {
		super(name, fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
