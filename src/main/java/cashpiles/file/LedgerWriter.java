package cashpiles.file;

import java.io.FileWriter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;

public class LedgerWriter extends FileWriter {

	public LedgerWriter(String filename) throws IOException {
		super(filename, StandardCharsets.UTF_8);
	}

}
