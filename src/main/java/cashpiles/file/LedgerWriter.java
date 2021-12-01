package cashpiles.file;

import java.io.BufferedWriter;
import java.io.IOException;
import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.BlankLine;
import cashpiles.ledger.Budget;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.IncomeBudgetEntry;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.RoutineBudgetEntry;

public class LedgerWriter implements ItemProcessor {

	private TableWriter table;
	private BufferedWriter writer;

	public LedgerWriter(BufferedWriter writer) {
		this.writer = writer;
	}

	@Override
	public void process(AccountBalance balance) {
		try {
			if (table != null) {
				table.flush();
				table = null;
			}
			writeDate(balance.date());
			writer.write(" balance ");
			writer.write(balance.account());
			writer.write("  ");
			writer.write(balance.amount().toString());
			writeComment(balance);
			writer.newLine();
		} catch (IOException ex) {
			// TODO handle it
		}
	}

	@Override
	public void process(AccountCommand command) {
		try {
			if (table != null) {
				table.flush();
				table = null;
			}
			writeDate(command.date());
			writer.write(" ");
			writer.write(switch (command.status()) {
			case ON_BUDGET -> "on-budget";
			case OFF_BUDGET -> "off-budget";
			case CLOSED -> "close";
			});
			writer.write(" ");
			writer.write(command.account());
			writeComment(command);
			writer.newLine();
		} catch (IOException ex) {
			// TODO handle it
		}
	}

	@Override
	public void process(BlankLine line) {
		try {
			if (table != null) {
				table.flush();
				table = null;
			}
			writeComment(line, false);
			writer.newLine();
		} catch (IOException ex) {
			// TODO do something
		}
	}

	@Override
	public boolean process(Budget budget) {
		try {
			if (table != null) {
				table.flush();
				table = null;
			}
			writeDate(budget.date());
			writer.write(" budget ");
			writer.write(budget.period().toString());
			writeComment(budget);
			writer.newLine();
		} catch (IOException ex) {
			// TODO handle it
		}
		return true;
	}

	@Override
	public void process(CloseBudgetEntry entry) {
		if (table == null) {
			table = new TableWriter(writer);
		}
		table.write(0, "  close ");
		table.write(1, entry.name());
		table.write(10, formatComment(entry));
		table.newLine();
	}

	@Override
	public void process(IncomeBudgetEntry entry) {
		if (table == null) {
			table = new TableWriter(writer);
		}
		table.write(0, "  income ");
		table.write(1, entry.name() + "  ");
		table.write(2, entry.owner());
		table.write(10, formatComment(entry));
		table.newLine();
	}

	@Override
	public void process(RoutineBudgetEntry entry) {
		if (table == null) {
			table = new TableWriter(writer);
		}
		table.write(0, "  routine ");
		table.write(1, entry.name() + "  ");
		table.write(2, entry.owner());
		table.write(10, formatComment(entry));
		table.newLine();
	}

	private String formatComment(LedgerItem item) {
		return formatComment(item, true);
	}

	private String formatComment(LedgerItem item, boolean pad) {
		var sb = new StringBuilder();
		if (item.comment().isEmpty()) {
			return sb.toString();
		}
		if (pad) {
			sb.append(' ');
		}
		sb.append("; ");
		sb.append(item.comment());
		return sb.toString();
	}

	private void writeComment(LedgerItem item) throws IOException {
		writeComment(item, true);
	}

	private void writeComment(LedgerItem item, boolean pad) throws IOException {
		writer.write(formatComment(item, pad));
	}

	private void writeDate(LocalDate date) throws IOException {
		// TODO get this from the same place as the LedgerReader scanner
		writer.write(date.format(DateTimeFormatter.ofPattern("M/d/yyyy")));
	}

}
