package cashpiles.account;

import java.util.Map;
import java.util.TreeMap;

import javax.swing.table.AbstractTableModel;

import cashpiles.currency.Amount;

@SuppressWarnings("serial")
class AccountsTableModel extends AbstractTableModel {

	private static final String[] headers = { "Account", "Balance" };
	final Map<String, StatementsTableModel> statements = new TreeMap<>();

	Amount balance() {
		var balance = new Amount();
		for (var entry : statements.entrySet()) {
			balance = balance.add(entry.getValue().balance());
		}
		return balance;
	}

	@Override
	public void fireTableDataChanged() {
		super.fireTableDataChanged();
		statements.forEach((k, v) -> v.fireTableDataChanged());
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
		return statements.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		return switch (col) {
		case 0 -> statements.keySet().toArray()[row];
		case 1 -> statements.values().stream().skip(row).findFirst().get().balance();
		default -> throw new IllegalArgumentException();
		};
	}

}
