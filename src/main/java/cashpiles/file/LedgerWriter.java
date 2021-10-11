package cashpiles.file;

import java.io.BufferedWriter;
import java.io.IOException;
import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.ItemProcessor;

public class LedgerWriter implements ItemProcessor {

	private BufferedWriter writer;

	public LedgerWriter(BufferedWriter writer) {
		this.writer = writer;
	}

	@Override
	public void process(AccountBalance balance) {
		try {
			writeDate(balance.date());
			writer.write(" balance ");
			writer.write(balance.account());
			writer.write("  ");
			writer.write(balance.amount().toString());
			writer.newLine();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	private void writeDate(LocalDate date) throws IOException {
		// TODO get this from the same place as the LedgerReader scanner
		writer.write(date.format(DateTimeFormatter.ofPattern("M/d/yyyy")));
	}

}
