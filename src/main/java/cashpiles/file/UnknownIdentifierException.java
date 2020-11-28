package cashpiles.file;

import cashpiles.file.FileReader.IdentifierType;

@SuppressWarnings("serial")
public class UnknownIdentifierException extends Exception {

	public UnknownIdentifierException(String identifier) {
		super("Unknown identifier '" + identifier + "'");
	}

	public UnknownIdentifierException(String identifier, IdentifierType type) {
		super("Unknown identifier '" + identifier + "' of type " + type);
	}

}
