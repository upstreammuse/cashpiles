package cashpiles.account;

import java.util.List;
import java.util.Map;

import javax.swing.table.AbstractTableModel;

import cashpiles.ledger.AccountTransactionEntry;

@SuppressWarnings("serial")
public class TransactionTableModel extends AbstractTableModel {

	private final Map<String, List<AccountStatement>> accounts;

	public TransactionTableModel(Map<String, List<AccountStatement>> accounts) {
		this.accounts = accounts;
	}

	@Override
	public int getColumnCount() {
		return 1;
	}

	@Override
	public int getRowCount() {
		int rows = 0;
		for (var account : accounts.values()) {
			for (var statement : account) {
				rows += statement.transactions.size();
			}
		}
		return rows;
	}

	@Override
	public Object getValueAt(int row, int col) {
		AccountTransactionEntry entry = null;
		counting: for (var account : accounts.values()) {
			for (var statement : account) {
				for (var transaction : statement.transactions) {
					if (row == 0) {
						entry = transaction;
						break counting;
					}
					row--;
				}
			}
		}
		if (entry == null) {
			throw new IllegalArgumentException();
		}

		return entry.parent.date;
	}

}
