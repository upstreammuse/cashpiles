package cashpiles.ledger;

public class OwnerTransactionEntry extends TrackingTransactionEntry {

	private String owner;

	public OwnerTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String owner() {
		return owner;
	}

	public OwnerTransactionEntry withOwner(String owner) {
		var retval = clone();
		retval.owner = owner;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) throws LedgerException {
		processor.process(this);
	}

	@Override
	public OwnerTransactionEntry clone() {
		return (OwnerTransactionEntry) super.clone();
	}

}
