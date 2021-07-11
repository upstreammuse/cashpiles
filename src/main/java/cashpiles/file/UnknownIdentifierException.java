package cashpiles.file;

import cashpiles.file.LedgerReader.IdentifierType;

@SuppressWarnings("serial")
public class UnknownIdentifierException extends Exception {

	UnknownIdentifierException(String identifier) {
		super("Unknown identifier '" + identifier + "'");
	}

	UnknownIdentifierException(String identifier, IdentifierType type) {
		super("Unknown identifier '" + identifier + "' of type " + type);
	}

}
