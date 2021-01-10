package cashpiles.ledger;

public class WithholdingBudgetEntry extends BudgetEntry {

	public String name;
	public String owner;

	public WithholdingBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
