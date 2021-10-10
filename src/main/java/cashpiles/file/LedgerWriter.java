package cashpiles.file;

import java.io.BufferedWriter;
import java.io.IOException;

import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.ItemProcessor;

public class LedgerWriter implements ItemProcessor {

	private BufferedWriter writer;

	public LedgerWriter(BufferedWriter writer) {
		this.writer = writer;
	}

	public void write(AccountBalance balance) throws IOException {
		writer.write(balance.date.toString());
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
