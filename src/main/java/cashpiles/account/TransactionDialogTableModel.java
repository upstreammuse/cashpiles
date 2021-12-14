package cashpiles.account;

import javax.swing.table.AbstractTableModel;

@SuppressWarnings("serial")
class TransactionDialogTableModel extends AbstractTableModel {

	private final static String[] headers = { "Account/Category/Owner", "Tracking Account", "Amount", "Notes" };

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
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		// TODO Auto-generated method stub
		return null;
	}

}
