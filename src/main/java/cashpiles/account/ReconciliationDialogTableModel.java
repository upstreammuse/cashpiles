package cashpiles.account;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import javax.swing.table.AbstractTableModel;

import cashpiles.ledger.AccountTransactionView;
import cashpiles.model.AccountTransactionsView;
import cashpiles.model.StatementsView;

@SuppressWarnings("serial")
class ReconciliationDialogTableModel extends AbstractTableModel {

	private static final String[] headers = { "Date", "Status", "Payee", "Amount" };

	private List<AccountTransactionView> filteredView = new ArrayList<>();
	private final AccountTransactionsView view;

	ReconciliationDialogTableModel(StatementsView view) {
		this.view = view.get(view.size() - 1);
	}

	@Override
	public int getColumnCount() {
		return headers.length;
	}

	@Override
	public String getColumnName(int col) {
		return headers[col];
	}

	@Override
	public int getRowCount() {
		return filteredView.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		var item = filteredView.get(row);
		return switch (col) {
		// TODO this feels like a hack, but also maybe a clever one
		case -1 -> item;
		case 0 -> item.date();
		case 1 -> item.accountStatus();
		case 2 -> item.payee();
		case 3 -> item.accountAmount();
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

	public void setDate(LocalDate newDate) {
		filteredView.clear();
		for (int i = 0; i < view.size(); i++) {
			var item = view.get(i);
			if (item.date().compareTo(newDate) <= 0) {
				filteredView.add(item);
			}
		}
		fireTableDataChanged();
	}

}
