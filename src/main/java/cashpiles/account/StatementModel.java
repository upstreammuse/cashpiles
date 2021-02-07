package cashpiles.account;

import java.util.List;

import javax.swing.ListSelectionModel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableModel;

@SuppressWarnings("serial")
public class StatementModel extends AbstractTableModel {

	private final List<AccountStatement> statements;

	public StatementModel(List<AccountStatement> statements) {
		this.statements = statements;
	}

	public TableModel getStatementModel(ListSelectionEvent event) {
		for (int i = event.getFirstIndex(); i <= event.getLastIndex(); i++) {
			if (((ListSelectionModel) event.getSource()).isSelectedIndex(i)) {
				return statements.get(i);
			}
		}
		// TODO make sure there's always at least one statement to select, even if it's
		// empty
		return null;
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
		case 0 -> statements.get(row).startDate();
		case 1 -> statements.get(row).endDate();
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

}
