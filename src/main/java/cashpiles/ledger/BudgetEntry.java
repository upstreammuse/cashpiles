package cashpiles.ledger;

public abstract class BudgetEntry extends LedgerItem {

	public String name;
	public Budget parent;

	public BudgetEntry(String name, String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
		this.name = name;
	}

	@Override
	abstract public void process(ItemProcessor processor);

}
