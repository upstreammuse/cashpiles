package cashpiles.account;

import java.util.Map;
import java.util.TreeMap;

import javax.swing.table.AbstractTableModel;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountTransactionEntry;

@SuppressWarnings("serial")
class AccountsTableModel extends AbstractTableModel {

	private final Map<String, Amount> accounts = new TreeMap<>();

	public void add(AccountTransactionEntry entry) throws AccountException {
		if (!accounts.containsKey(entry.account)) {
			throw AccountException.forUnknown(entry);
		}
		accounts.put(entry.account, accounts.get(entry.account).add(entry.amount));
		fireTableDataChanged();
	}

	void close(String account) {
		accounts.remove(account);
		fireTableDataChanged();
	}

	public void open(String account) {
		accounts.put(account, new Amount());
		fireTableDataChanged();
	}

	@Override
	public int getColumnCount() {
		return 2;
	}

	@Override
	public int getRowCount() {
		return accounts.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		return switch (col) {
		case 0 -> accounts.keySet().toArray()[row];
		case 1 -> accounts.values().toArray()[row];
		default -> throw new IllegalArgumentException();
		};
	}

}
