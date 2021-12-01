package cashpiles.ledger;

import cashpiles.file.LedgerReader.IdentifierType;

@SuppressWarnings("serial")
public class LedgerException extends Exception {

	public static LedgerException forIdentifierMismatch(LedgerItem item, String identifier, IdentifierType type,
			IdentifierType expected) {
		return new LedgerException(item, expected + " '" + identifier + "' used as " + type);
	}

	public static LedgerException forInvalidContent(String line, int lineNumber) {
		return new LedgerException(new BlankLine(null, lineNumber, null), "Invalid content '" + line + "'");
	}

	public static LedgerException forUnknownIdentifier(String identifier, int lineNumber) {
		return new LedgerException(new BlankLine(null, lineNumber, null), "Unknown identifier '" + identifier + "'");
	}

	public static LedgerException forUnknownIdentifier(LedgerItem item, String identifier, IdentifierType type) {
		return new LedgerException(item, "Unknown identifier '" + identifier + "' of type " + type);
	}

	protected LedgerException(LedgerItem item, String message) {
		super(message + " (line " + item.lineNumber() + ")");
	}

}
