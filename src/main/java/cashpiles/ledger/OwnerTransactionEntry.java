package cashpiles.ledger;

public class OwnerTransactionEntry extends TrackingTransactionEntry {

	private String owner;

	public OwnerTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public OwnerTransactionEntry(OwnerTransactionEntry other) {
		super(other);
		owner = other.owner;
	}

	public String owner() {
		return owner;
	}

	public OwnerTransactionEntry withOwner(String owner) {
		var retval = new OwnerTransactionEntry(this);
		retval.owner = owner;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
