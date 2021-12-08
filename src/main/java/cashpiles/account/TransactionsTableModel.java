package cashpiles.account;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import javax.swing.table.AbstractTableModel;

import cashpiles.currency.Amount;
import cashpiles.model.TransactionParticle;

@SuppressWarnings("serial")
class TransactionsTableModel extends AbstractTableModel {

	Optional<LocalDate> endDate = Optional.empty();
	private final Amount startBalance;
	final List<TransactionParticle> transactions = new ArrayList<>();

	TransactionsTableModel(Amount startBalance) {
		this.startBalance = startBalance;
	}

	Amount balance() {
		return transactions.stream().map(transaction -> transaction.amount()).reduce(startBalance,
				(total, amount) -> total.add(amount));
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
		case 0 -> xact.date();
		case 1 -> xact.status();
		case 2 -> xact.payee();
		case 3 -> xact.amount();
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

}
