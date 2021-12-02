package cashpiles.file;

import java.io.BufferedWriter;
import java.io.IOException;
import java.text.NumberFormat;
import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.BlankLine;
import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.ledger.IncomeBudgetEntry;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.ManualGoalBudgetEntry;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.ReserveBudgetEntry;
import cashpiles.ledger.RoutineBudgetEntry;
import cashpiles.ledger.Transaction;
import cashpiles.ledger.WithholdingBudgetEntry;

public class LedgerWriter implements ItemProcessor {

	private TableWriter table;
	private BufferedWriter writer;

	public LedgerWriter(BufferedWriter writer) {
		this.writer = writer;
	}

	@Override
	public void finish() {
		try {
			if (table != null) {
				table.flush();
				table = null;
			}
		} catch (IOException ex) {
			// TODO handle it
		}
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
	public void process(AccountTransactionEntry entry) {
		if (table == null) {
			table = new TableWriter(writer);
		}
		table.write(0, "  " + entry.account() + "  ");
		table.write(2, entry.amount().toString(), TableWriter.Alignment.ALIGN_RIGHT);
		table.write(10, formatComment(entry));
		table.newLine();
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
	public void process(CategoryTransactionEntry entry) {
		if (table == null) {
			table = new TableWriter(writer);
		}
		table.write(0, "  " + entry.category() + "  ");
		entry.trackingAccount().ifPresent(account -> table.write(1, account + "  "));
		table.write(2, entry.amount().toString(), TableWriter.Alignment.ALIGN_RIGHT);
		table.write(10, formatComment(entry));
		table.newLine();
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
	public void process(GoalBudgetEntry entry) {
		if (table == null) {
			table = new TableWriter(writer);
		}
		table.write(0, "  goal ");
		table.write(1, entry.name() + "  ");
		table.write(2, entry.owner() + "  ");
		table.write(3, entry.amount().toString() + " ", TableWriter.Alignment.ALIGN_RIGHT);
		table.write(4, formatDate(entry.dates().endDate()) + " ");
		if (entry.repeat()) {
			table.write(5, "repeat");
		}
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
	public void process(ManualGoalBudgetEntry entry) {
		if (table == null) {
			table = new TableWriter(writer);
		}
		table.write(0, "  goal ");
		table.write(1, entry.name() + "  ");
		table.write(2, entry.owner());
		table.write(10, formatComment(entry));
		table.newLine();
	}

	@Override
	public void process(OwnerTransactionEntry entry) {
		if (table == null) {
			table = new TableWriter(writer);
		}
		table.write(0, "  " + entry.owner() + "  ");
		entry.trackingAccount().ifPresent(account -> table.write(1, account + "  "));
		table.write(2, entry.amount().toString(), TableWriter.Alignment.ALIGN_RIGHT);
		table.write(10, formatComment(entry));
		table.newLine();
	}

	@Override
	public void process(ReserveBudgetEntry entry) {
		if (table == null) {
			table = new TableWriter(writer);
		}
		table.write(0, "  reserve ");
		table.write(1, entry.name() + "  ");
		table.write(2, entry.owner() + "  ");
		table.write(3, NumberFormat.getPercentInstance().format(entry.percentage()) + " ",
				TableWriter.Alignment.ALIGN_RIGHT);
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

	@Override
	public boolean process(Transaction xact) {
		try {
			if (table != null) {
				table.flush();
				table = null;
			}
			writeDate(xact.date());
			writer.write(" ");
			writer.write(switch (xact.status()) {
			case CLEARED -> "*";
			case DISPUTED -> "!";
			case PENDING -> "?";
			});
			writer.write(" " + xact.payee() + "  ");
			writer.write(xact.total().toString());
			writeComment(xact);
			writer.newLine();
		} catch (IOException ex) {
			// TODO handle it
		}
		return true;
	}

	@Override
	public void process(WithholdingBudgetEntry entry) {
		if (table == null) {
			table = new TableWriter(writer);
		}
		table.write(0, "  withholding ");
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
		sb.append(";");
		sb.append(item.comment());
		return sb.toString();
	}

	private String formatDate(LocalDate date) {
		// TODO get this from the same place as the LedgerReader scanner
		return date.format(DateTimeFormatter.ofPattern("M/d/yyyy"));
	}

	private void writeComment(LedgerItem item) throws IOException {
		writeComment(item, true);
	}

	private void writeComment(LedgerItem item, boolean pad) throws IOException {
		writer.write(formatComment(item, pad));
	}

	private void writeDate(LocalDate date) throws IOException {
		// TODO get this from the same place as the LedgerReader scanner
		writer.write(formatDate(date));
	}

}
