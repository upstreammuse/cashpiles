package cashpiles.account;

import javax.swing.table.AbstractTableModel;

import cashpiles.ledger.AccountCommand;
import cashpiles.model2.AccountsView;

@SuppressWarnings("serial")
class AccountsTableModel extends AbstractTableModel {

	private static final String[] headers = { "Account", "Balance" };

	private final AccountCommand.Status filter;
	private final AccountsView view;

	AccountsTableModel(AccountsView view, AccountCommand.Status filter) {
		this.filter = filter;
		this.view = view;
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
		return view.size(filter);
	}

	@Override
	public Object getValueAt(int row, int col) {
		return switch (col) {
		case 0 -> view.name(filter, row);
		case 1 -> view.balance(filter, row);
		default -> throw new IllegalArgumentException();
		};
	}

}
