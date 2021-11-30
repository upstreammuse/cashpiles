package cashpiles.ledger;

public class RoutineBudgetEntry extends BudgetEntry<RoutineBudgetEntry> {

	private String owner;

	public RoutineBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public RoutineBudgetEntry(RoutineBudgetEntry other) {
		super(other);
		owner = other.owner;
	}

	public String owner() {
		return owner;
	}

	public RoutineBudgetEntry withOwner(String owner) {
		var retval = duplicate();
		retval.owner = owner;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

	@Override
	RoutineBudgetEntry asChild() {
		return this;
	}

	@Override
	RoutineBudgetEntry duplicate() {
		return new RoutineBudgetEntry(this);
	}

}
