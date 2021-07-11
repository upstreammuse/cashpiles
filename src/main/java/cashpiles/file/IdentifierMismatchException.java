package cashpiles.file;

import cashpiles.file.LedgerReader.IdentifierType;

@SuppressWarnings("serial")
public class IdentifierMismatchException extends Exception {

	IdentifierMismatchException(String identifier, IdentifierType type, IdentifierType expected) {
		super(expected + " '" + identifier + "' used as " + type);
	}

}
