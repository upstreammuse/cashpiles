package cashpiles.account;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import javax.swing.table.AbstractTableModel;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;
import cashpiles.util.Lists;

@SuppressWarnings("serial")
public class AccountStatement extends AbstractTableModel {

	// TODO this should really be able to go away
	private static class Transaction {
		Amount amount;
		LocalDate date;
		String payee;
		cashpiles.ledger.Transaction.Status status;
	}

	private final Amount startBalance;
	private final LocalDate startDate;
	private final AccountCommand.Status status;
	private final List<Transaction> transactions = new ArrayList<>();

	AccountStatement(LocalDate startDate, AccountCommand.Status status, Amount startBalance) {
		this.startDate = startDate;
		this.status = status;
		this.startBalance = startBalance;
	}

	public void add(AccountTransactionEntry entry) throws AccountException {
		switch (status) {
		case ON_BUDGET -> {
		}
		case CLOSED -> throw AccountException.forClosed(entry);
		case OFF_BUDGET -> throw AccountException.forAccountWithOffBudget(entry);
		}
		var xact = new Transaction();
		xact.date = entry.parent().date();
		xact.status = entry.parent().status();
		xact.payee = entry.parent().payee();
		xact.amount = entry.amount();
		transactions.add(xact);
	}

	public void add(TrackingTransactionEntry entry) throws AccountException {
		switch (status) {
		case ON_BUDGET -> throw AccountException.forTrackingWithBudget(entry);
		case CLOSED -> throw AccountException.forClosed(entry);
		case OFF_BUDGET -> {
		}
		}
		var xact = new Transaction();
		xact.date = entry.parent().date();
		xact.status = entry.parent().status();
		xact.payee = entry.parent().payee();
		xact.amount = entry.amount().negate();
		transactions.add(xact);
	}

	public void add(UnbalancedTransaction transaction) throws AccountException {
		switch (status) {
		case ON_BUDGET -> throw AccountException.forUnbalancedTransactionWithOnBudgetAccount(transaction);
		case CLOSED -> throw AccountException.forClosed(transaction);
		case OFF_BUDGET -> {
		}
		}
		var xact = new Transaction();
		xact.date = transaction.date();
		xact.status = transaction.status();
		xact.payee = transaction.payee();
		xact.amount = transaction.amount();
		transactions.add(xact);
	}

	public Amount balance() {
		return transactions.stream().map(transaction -> transaction.amount).reduce(startBalance,
				(total, amount) -> total.add(amount));
	}

	public AccountCommand.Status status() {
		return status;
	}

	public LocalDate startDate() {
		return startDate;
	}

	// TODO this is wrong, should be the date of the balance command that ends the
	// statement
	public LocalDate endDate() {
		if (transactions.isEmpty()) {
			return startDate;
		}
		return Lists.lastOf(transactions).date;
	}

	@Override
	public int getColumnCount() {
		return 4;
	}

	@Override
	public int getRowCount() {
		return transactions.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		var xact = transactions.get(row);
		return switch (col) {
		case 0 -> xact.date;
		case 1 -> xact.status;
		case 2 -> xact.payee;
		case 3 -> xact.amount;
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

	public void setEndDate(LocalDate date) {
		// TODO Auto-generated method stub

	}

}
