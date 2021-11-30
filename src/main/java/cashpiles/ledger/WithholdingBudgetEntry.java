package cashpiles.ledger;

public class WithholdingBudgetEntry extends BudgetEntry<WithholdingBudgetEntry> {

	private String owner;

	public WithholdingBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public WithholdingBudgetEntry(WithholdingBudgetEntry other) {
		super(other);
		owner = other.owner;
	}

	public String owner() {
		return owner;
	}

	public WithholdingBudgetEntry withOwner(String owner) {
		var retval = duplicate();
		retval.owner = owner;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

	@Override
	WithholdingBudgetEntry asChild() {
		return this;
	}

	@Override
	WithholdingBudgetEntry duplicate() {
		return new WithholdingBudgetEntry(this);
	}

}
