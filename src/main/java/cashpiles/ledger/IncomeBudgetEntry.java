package cashpiles.ledger;

public class IncomeBudgetEntry extends BudgetEntry {

	private String owner;

	public IncomeBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String owner() {
		return owner;
	}

	public IncomeBudgetEntry withOwner(String owner) {
		var retval = clone();
		retval.owner = owner;
		return retval;
	}

	@Override
	public IncomeBudgetEntry withName(String name) {
		return (IncomeBudgetEntry) super.withName(name);
	}

	@Override
	public void process(ItemProcessor processor) throws LedgerException {
		processor.process(this);
	}

	@Override
	public IncomeBudgetEntry clone() {
		return (IncomeBudgetEntry) super.clone();
	}

}
