package cashpiles.ledger;

public class WithholdingBudgetEntry extends BudgetEntry {

	private String owner;

	public WithholdingBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String owner() {
		return owner;
	}

	public WithholdingBudgetEntry withOwner(String owner) {
		var retval = clone();
		retval.owner = owner;
		return retval;
	}

	@Override
	public WithholdingBudgetEntry withName(String name) {
		return (WithholdingBudgetEntry) super.withName(name);
	}

	@Override
	public void process(ItemProcessor processor) throws LedgerException {
		processor.process(this);
	}

	@Override
	public WithholdingBudgetEntry clone() {
		return (WithholdingBudgetEntry) super.clone();
	}

}
