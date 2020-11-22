package cashpiles.ledger;

public class LedgerItem {
	
	protected final String comment;
	protected final String fileName;
	protected final int lineNumber;
	
	public LedgerItem(String fileName, int lineNumber, String comment) {
		this.comment = comment;
		this.fileName = fileName;
		this.lineNumber = lineNumber;
	}

}
