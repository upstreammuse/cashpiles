package cashpiles.file;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;

import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.ItemProcessor;

public class LedgerWriter extends FileWriter implements ItemProcessor {

	public LedgerWriter(String filename) throws IOException {
		super(filename, StandardCharsets.UTF_8);
	}

	public void write(AccountBalance balance) throws IOException {
		try (var buf = new BufferedWriter(this)) {
			buf.write(balance.date.toString());
		}
	}

	@Override
	public void process(AccountBalance balance) {
		try {
			write(balance);
		} catch (Exception ex) {
			// TODO now what? Error dialog?
		}
	}

}
