package cashpiles.account;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import javax.swing.table.AbstractTableModel;

import cashpiles.currency.Amount;
import cashpiles.ledger.Account;
import cashpiles.util.Lists;

@SuppressWarnings("serial")
class AccountsTableModel extends AbstractTableModel {

	private final Map<String, List<AccountStatement>> accounts;
	private final ArrayList<String> cache = new ArrayList<>();
	private final Account.Status filter;

	AccountsTableModel(Map<String, List<AccountStatement>> accounts, Account.Status filter) {
		this.accounts = accounts;
		this.filter = filter;
	}

	public Amount balance() {
		var balance = new Amount();
		for (int i = 0; i < getRowCount(); i++) {
			balance = balance.add((Amount) getValueAt(i, 1));
		}
		return balance;
	}

	@Override
	public int getColumnCount() {
		return 2;
	}

	@Override
	public int getRowCount() {
		cache.clear();
		for (var entry : accounts.entrySet()) {
			if (Lists.lastOf(entry.getValue()).status() == filter) {
				cache.add(entry.getKey());
			}
		}
		return cache.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		return switch (col) {
		case 0 -> cache.get(row);
		case 1 -> Lists.lastOf(accounts.get(cache.get(row))).balance();
		default -> throw new IllegalArgumentException();
		};
	}

}
