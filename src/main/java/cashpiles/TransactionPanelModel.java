package cashpiles;

import java.util.ArrayList;
import java.util.List;

import javax.swing.table.AbstractTableModel;

import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.Transaction;

@SuppressWarnings("serial")
class TransactionPanelModel extends AbstractTableModel implements ItemProcessor {

	private final List<Transaction> xacts = new ArrayList<>();

	void clear() {
		xacts.clear();
	}

	@Override
	public String getColumnName(int col) {
		return switch (col) {
		case 0 -> "Date";
		case 1 -> "Status";
		case 2 -> "Payee";
		case 3 -> "Total";
		default -> throw new IllegalArgumentException();
		};
	}

	@Override
	public int getRowCount() {
		return xacts.size();
	}

	@Override
	public int getColumnCount() {
		return 4;
	}

	@Override
	public Object getValueAt(int row, int col) {
		return switch (col) {
		case 0 -> xacts.get(row).date();
		case 1 -> xacts.get(row).status();
		case 2 -> xacts.get(row).payee();
		case 3 -> xacts.get(row).total();
		default -> throw new IllegalArgumentException();
		};
	}

	@Override
	public void finish() {
		fireTableDataChanged();
	}

	@Override
	public boolean process(Transaction xact) {
		xacts.add(xact);
		return false;
	}

}
