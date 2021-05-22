package cashpiles.ledger;

public class WithholdingBudgetEntry extends BudgetEntry {

	public String owner;

	public WithholdingBudgetEntry(String name, String fileName, int lineNumber, String comment) {
		super(name, fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
