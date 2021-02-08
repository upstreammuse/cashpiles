package cashpiles.account;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import javax.swing.JTable;
import javax.swing.event.ListSelectionEvent;
import javax.swing.table.AbstractTableModel;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountCommand.Status;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;
import cashpiles.util.Lists;

@SuppressWarnings("serial")
public class Account extends AbstractTableModel {

	private final List<AccountStatement> statements = new ArrayList<>();
	private Status status = Status.CLOSED;
	private Optional<JTable> transactionsUI = Optional.empty();

	public Amount balance() {
		if (statements.isEmpty()) {
			return new Amount();
		}
		return Lists.lastOf(statements).balance();
	}

	// TODO should this be an account exception? Should they all be LedgerException?
	public void closeStatement(AccountBalance balance) throws BalanceException {
		if (!balance().equals(balance.amount)) {
			throw BalanceException.forUnbalanced(balance, balance());
		}
		// TODO could make it so you can't balance a closed account, which would let
		// this be an exception instead of making a dummy statement
		if (statements.isEmpty()) {
			statements.add(new AccountStatement(balance.date, status, new Amount()));
		}
		var lastStatement = Lists.lastOf(statements);
		lastStatement.setEndDate(balance.date);
		statements.add(new AccountStatement(balance.date, status, lastStatement.balance()));
	}

	public Status status() {
		return status;
	}

	public void update(AccountCommand command) throws AccountException {
		switch (status) {
		case ON_BUDGET -> {
			switch (command.status) {
			case ON_BUDGET -> throw AccountException.forAlreadyOpen(command);
			case OFF_BUDGET -> throw AccountException.forTypeChange(command);
			case CLOSED -> {
				if (!balance().equals(new Amount())) {
					throw AccountException.forNonZeroClose(command);
				}
				Lists.lastOf(statements).setEndDate(command.date);
				status = Status.CLOSED;
			}
			}
		}
		case OFF_BUDGET -> {
			switch (command.status) {
			case ON_BUDGET -> throw AccountException.forTypeChange(command);
			case OFF_BUDGET -> throw AccountException.forAlreadyOpen(command);
			case CLOSED -> {
				// TODO DRY this out
				if (!balance().equals(new Amount())) {
					throw AccountException.forNonZeroClose(command);

				}
				Lists.lastOf(statements).setEndDate(command.date);
				status = Status.CLOSED;
			}
			}
		}
		case CLOSED -> {
			switch (command.status) {
			case ON_BUDGET -> {
				status = Status.ON_BUDGET;
				statements.add(new AccountStatement(command.date, status, new Amount()));
			}
			case OFF_BUDGET -> {
				status = Status.OFF_BUDGET;
				statements.add(new AccountStatement(command.date, status, new Amount()));
			}
			case CLOSED -> throw AccountException.forAlreadyClosed(command);
			}
		}
		}
	}

	public void add(AccountTransactionEntry entry) throws LedgerException {
		if (statements.isEmpty()) {
			throw AccountException.forClosed(entry);
		}
		Lists.lastOf(statements).add(entry);
	}

	public void add(TrackingTransactionEntry entry) throws LedgerException {
		if (statements.isEmpty()) {
			throw AccountException.forClosed(entry);
		}
		Lists.lastOf(statements).add(entry);
	}

	public void add(UnbalancedTransaction transaction) throws LedgerException {
		if (statements.isEmpty()) {
			throw AccountException.forClosed(transaction);
		}
		Lists.lastOf(statements).add(transaction);
	}

	public void forTransactions(JTable transactionsUI) {
		this.transactionsUI = Optional.of(transactionsUI);
	}

	@Override
	public int getColumnCount() {
		return 2;
	}

	@Override
	public int getRowCount() {
		return statements.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		return switch (col) {
		case 0 -> statements.get(row).startDate();
		case 1 -> statements.get(row).endDate();
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

	public void selectStatement(int i) {
		transactionsUI.ifPresent(ui -> {
			ui.setModel(statements.get(i));
		});
	}

}
