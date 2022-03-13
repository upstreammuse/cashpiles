package cashpiles.ledger;

// TODO the fileName field is not used by anything, and can be removed if there's no use for it going forward
public abstract class LedgerItem implements Cloneable {

	private final String comment;
	private final String fileName;
	private final int lineNumber;

	public LedgerItem(String fileName, int lineNumber, String comment) {
		this.comment = comment;
		this.fileName = fileName;
		this.lineNumber = lineNumber;
	}

	public String comment() {
		return comment;
	}

	public String fileName() {
		return fileName;
	}

	public int lineNumber() {
		return lineNumber;
	}

	public abstract void process(ItemProcessor processor) throws LedgerException;

	@Override
	public LedgerItem clone() {
		try {
			return (LedgerItem) super.clone();
		} catch (CloneNotSupportedException ex) {
			throw new RuntimeException(ex);
		}
	}

}
