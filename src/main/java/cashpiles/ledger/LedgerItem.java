package cashpiles.ledger;

import java.util.UUID;

// TODO the fileName field is not used by anything, and can be removed if there's no use for it going forward
public abstract class LedgerItem implements Cloneable {

	private final String comment;
	private final String fileName;
	private final UUID id = UUID.randomUUID();
	private final int lineNumber;

	public LedgerItem(String fileName, int lineNumber, String comment) {
		this.comment = comment;
		this.fileName = fileName;
		this.lineNumber = lineNumber;
	}

	public String comment() {
		return comment;
	}

	@Override
	public boolean equals(Object other) {
		if (other == null || !(other instanceof LedgerItem)) {
			return false;
		}
		return id.equals(((LedgerItem) other).id);
	}

	public String fileName() {
		return fileName;
	}

	@Override
	public int hashCode() {
		return id.hashCode();
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
