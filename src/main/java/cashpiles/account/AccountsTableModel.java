package cashpiles.account;

import java.util.Map;
import java.util.TreeMap;

import javax.swing.table.AbstractTableModel;

import cashpiles.currency.Amount;
import cashpiles.util.Lists;

@SuppressWarnings("serial")
class AccountsTableModel extends AbstractTableModel {

	final Map<String, StatementsTableModel> statements = new TreeMap<>();

	public Amount balance() {
		var balance = new Amount();
		for (var entry : statements.entrySet()) {
			balance = balance.add(Lists.lastOf(entry.getValue().transactionModels).balance());
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
		return 2;
	}

	@Override
	public int getRowCount() {
		return statements.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		return switch (col) {
		case 0 -> statements.keySet().toArray()[row];
		case 1 -> Lists.lastOf(statements.values().stream().skip(row).findFirst().get().transactionModels).balance();
		default -> throw new IllegalArgumentException();
		};
	}

}
