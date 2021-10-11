package cashpiles.ledger;

public abstract class LedgerItem {

	private final String comment;
	private final String fileName;
	private final int lineNumber;

	public LedgerItem(String fileName, int lineNumber, String comment) {
		this.comment = comment;
		this.fileName = fileName;
		this.lineNumber = lineNumber;
	}

	public LedgerItem(LedgerItem other) {
		comment = other.comment;
		fileName = other.fileName;
		lineNumber = other.lineNumber;
	}

	public String comment() {
		return comment;
	}

	public int lineNumber() {
		return lineNumber;
	}

	public abstract void process(ItemProcessor processor);

}
