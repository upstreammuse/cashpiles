package cashpiles.ledger;

public class BlankLine extends LedgerItem {

	public BlankLine(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
