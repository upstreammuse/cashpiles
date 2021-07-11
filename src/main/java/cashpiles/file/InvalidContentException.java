package cashpiles.file;

@SuppressWarnings("serial")
public class InvalidContentException extends Exception {

	InvalidContentException(String fileName, int lineNumber, String line) {
		super("Invalid line '" + line + "' in file '" + fileName + "', line " + lineNumber);
	}

}
