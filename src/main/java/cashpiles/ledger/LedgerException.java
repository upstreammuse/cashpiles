package cashpiles.ledger;

@SuppressWarnings("serial")
public class LedgerException extends Exception {

	protected LedgerException(LedgerItem item, String message) {
		super(message + " (line " + item.lineNumber() + ")");
	}

}
