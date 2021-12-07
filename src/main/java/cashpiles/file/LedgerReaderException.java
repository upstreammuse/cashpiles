package cashpiles.file;

import cashpiles.file.LedgerReader.IdentifierType;
import cashpiles.ledger.BlankLine;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;

@SuppressWarnings("serial")
public class LedgerReaderException extends LedgerException {

	static LedgerReaderException forIdentifierMismatch(LedgerItem item, String identifier, IdentifierType type,
			IdentifierType expected) {
		return new LedgerReaderException(item, expected + " '" + identifier + "' used as " + type);
	}

	static LedgerReaderException forInvalidContent(String line, int lineNumber) {
		return new LedgerReaderException(new BlankLine(null, lineNumber, null), "Invalid content '" + line + "'");
	}

	static LedgerReaderException forUnknownIdentifier(String identifier, int lineNumber) {
		return new LedgerReaderException(new BlankLine(null, lineNumber, null),
				"Unknown identifier '" + identifier + "'");
	}

	static LedgerReaderException forUnknownIdentifier(LedgerItem item, String identifier, IdentifierType type) {
		return new LedgerReaderException(item, "Unknown identifier '" + identifier + "' of type " + type);
	}

	protected LedgerReaderException(LedgerItem item, String message) {
		super(item, message);
	}

}
