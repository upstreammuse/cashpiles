package cashpiles.ledger;

public class IncomeBudgetEntry extends BudgetEntry<IncomeBudgetEntry> {

	private String owner;

	public IncomeBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public IncomeBudgetEntry(IncomeBudgetEntry other) {
		super(other);
		owner = other.owner;
	}

	public String owner() {
		return owner;
	}

	public IncomeBudgetEntry withOwner(String owner) {
		var retval = duplicate();
		retval.owner = owner;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

	@Override
	IncomeBudgetEntry asChild() {
		return this;
	}

	@Override
	IncomeBudgetEntry duplicate() {
		return new IncomeBudgetEntry(this);
	}

}
