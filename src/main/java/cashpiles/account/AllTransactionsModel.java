package cashpiles.account;

import java.util.ArrayList;
import java.util.List;

import javax.swing.table.AbstractTableModel;

import cashpiles.ledger.TransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;

@SuppressWarnings("serial")
public class AllTransactionsModel extends AbstractTableModel {

	private final List<TransactionEntry> entries = new ArrayList<>();

	public void add(TransactionEntry entry) {
		entries.add(entry);
		fireTableDataChanged();
	}

	@Override
	public int getColumnCount() {
		return 4;
	}

	@Override
	public int getRowCount() {
		return entries.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		var entry = entries.get(row);
		return switch (col) {
		case 0 -> entry.parent.date;
		case 1 -> entry.parent.status;
		case 2 -> entry.parent.payee;
		case 3 -> entry.amount;
		default -> throw new IllegalArgumentException();
		};
	}

	public void add(UnbalancedTransaction transaction) {
		// TODO Auto-generated method stub
		
	}

}
