package cashpiles.ledger;

public class CloseBudgetEntry extends BudgetEntry<CloseBudgetEntry> {

	public CloseBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public CloseBudgetEntry(CloseBudgetEntry other) {
		super(other);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

	@Override
	CloseBudgetEntry asChild() {
		return this;
	}

	@Override
	CloseBudgetEntry duplicate() {
		return new CloseBudgetEntry(this);
	}

}
