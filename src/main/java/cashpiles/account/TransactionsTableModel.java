package cashpiles.account;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import javax.swing.table.AbstractTableModel;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.model.TransactionParticle;

@SuppressWarnings("serial")
class TransactionsTableModel extends AbstractTableModel {

	private Optional<LocalDate> endDate = Optional.empty();
	private static final String[] headers = { "Date", "Status", "Payee", "Amount" };
	private final Amount startBalance;
	final List<TransactionParticle> transactions = new ArrayList<>();

	TransactionsTableModel(Amount startBalance) {
		this.startBalance = startBalance;
	}

	Amount balance() {
		return transactions.stream().map(transaction -> transaction.amount()).reduce(startBalance,
				(total, amount) -> total.add(amount));
	}

	Optional<LocalDate> endDate() {
		return endDate;
	}

	TransactionsTableModel reconcile(AccountBalance balance) {
		assert (balance().equals(balance.amount())) : "Account balance was not properly validated";
		endDate = Optional.of(balance.date());
		return new TransactionsTableModel(balance.amount());
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
		return transactions.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		var xact = transactions.get(row);
		return switch (col) {
		case 0 -> xact.date();
		case 1 -> xact.status();
		case 2 -> xact.payee();
		case 3 -> xact.amount();
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

}
