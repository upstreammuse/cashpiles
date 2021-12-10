package cashpiles.account;

import java.time.LocalDate;
import java.util.Optional;

import javax.swing.table.AbstractTableModel;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.model.LedgerModelException;
import cashpiles.model.Statement;
import cashpiles.model.TransactionParticle;

@SuppressWarnings("serial")
class TransactionsTableModel extends AbstractTableModel {

	private static final String[] headers = { "Date", "Status", "Payee", "Amount" };

	private Statement statement;

	TransactionsTableModel(Amount startBalance) {
		statement = new Statement(startBalance);
	}

	void add(TransactionParticle transaction) {
		statement = statement.withTransaction(transaction);
	}

	Amount balance() {
		return statement.balance();
	}

	Optional<LocalDate> closingDate() {
		return statement.closingDate();
	}

	TransactionsTableModel reconcile(AccountBalance balance) {
		try {
			var reconciled = statement.withReconciliation(balance);
			var retval = new TransactionsTableModel(reconciled.balance());
			retval.statement = statement.withReconciliationRemainder(reconciled);
			statement = reconciled;
			return retval;
		} catch (LedgerModelException ex) {
			// TODO by the time the UI gets this data, it should already be validated by the
			// main model, so if there is a model exception here, we need to do something to
			// make it obvious the program is broken
			throw new RuntimeException(ex);
		}
	}

	@Override
	public int getColumnCount() {
		return headers.length;
	}

	@Override
	public String getColumnName(int index) {
		return headers[index];
	}

	@Override
	public int getRowCount() {
		return statement.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		var xact = statement.get(row);
		return switch (col) {
		case 0 -> xact.date();
		case 1 -> xact.status();
		case 2 -> xact.payee();
		case 3 -> xact.amount();
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

}
