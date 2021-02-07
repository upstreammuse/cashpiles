package cashpiles.ledger;

public class OwnerTransactionEntry extends TrackingTransactionEntry {

	public String owner;

	public OwnerTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
