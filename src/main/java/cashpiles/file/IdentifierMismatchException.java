package cashpiles.file;

import cashpiles.file.FileReader.IdentifierType;

public class IdentifierMismatchException extends Exception {

	public IdentifierMismatchException(String identifier, IdentifierType type, IdentifierType expected) {
		super(expected + " '" + identifier + "' used as " + type);
	}

}
