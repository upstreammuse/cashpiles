package cashpiles.ledger;

public abstract class BudgetEntry extends LedgerItem {

	public Budget parent;

	public BudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	abstract public void process(ItemProcessor processor);

}
