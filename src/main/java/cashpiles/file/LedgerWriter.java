package cashpiles.file;

import java.io.BufferedWriter;

import cashpiles.ledger.ItemProcessor;

public class LedgerWriter implements ItemProcessor {

	private BufferedWriter writer;

	public LedgerWriter(BufferedWriter writer) {
		this.writer = writer;
	}

}
