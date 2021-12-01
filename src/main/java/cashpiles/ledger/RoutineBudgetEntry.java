package cashpiles.ledger;

public class RoutineBudgetEntry extends BudgetEntry {

	private String owner;

	public RoutineBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String owner() {
		return owner;
	}

	public RoutineBudgetEntry withOwner(String owner) {
		var retval = clone();
		retval.owner = owner;
		return retval;
	}

	@Override
	public RoutineBudgetEntry withName(String name) {
		return (RoutineBudgetEntry) super.withName(name);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

	@Override
	public RoutineBudgetEntry clone() {
		return (RoutineBudgetEntry) super.clone();
	}

}
