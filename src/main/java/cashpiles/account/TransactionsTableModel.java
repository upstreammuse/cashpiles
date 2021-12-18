package cashpiles.account;

import java.util.Optional;

import javax.swing.table.AbstractTableModel;

import cashpiles.model.AccountTransactionsView;

@SuppressWarnings("serial")
class TransactionsTableModel extends AbstractTableModel {

	private static final String[] headers = { "Date", "Status", "Payee", "Amount" };

	private final Optional<AccountTransactionsView> view;

	TransactionsTableModel() {
		view = Optional.empty();
	}

	TransactionsTableModel(AccountTransactionsView view) {
		this.view = Optional.of(view);
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
		return view.map(view -> view.size()).orElse(0);
	}

	@Override
	public Object getValueAt(int row, int col) {
		var xact = view.orElseThrow().get(row);
		return switch (col) {
		case 0 -> xact.date();
		case 1 -> xact.accountStatus();
		case 2 -> xact.payee();
		case 3 -> xact.accountAmount();
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

}
