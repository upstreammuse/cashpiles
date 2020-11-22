package cashpiles.budget.ui;

import java.util.List;
import java.util.ArrayList;

import javax.swing.table.AbstractTableModel;

import cashpiles.budget.BudgetPeriod;
import cashpiles.util.Lists;

class BudgetWindowTableModel extends AbstractTableModel {

	private final List<BudgetPeriod> periods = new ArrayList<>();

	BudgetWindowTableModel(BudgetPeriod start) {
		periods.add(start);
		fireTableDataChanged();
	}

	void generateNext() {
		periods.add(Lists.lastOf(periods).next());
		fireTableDataChanged();
	}

	@Override
	public int getColumnCount() {
		return 5;
	}

	@Override
	public String getColumnName(int index) {
		String[] headers = { "Start Date", "End Date", "Allocation", "Activity", "Balance" };
		return headers[index];
	}

	@Override
	public int getRowCount() {
		return periods.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		var period = periods.get(row);
		var dates = period.dates();
		if (col == 0) {
			return dates.startDate();
		} else if (col == 1) {
			return dates.endDate();
		} else if (col == 2) {
			return period.allocation();
		} else if (col == 3) {
			return period.activity();
		} else if (col == 4) {
			return period.balance();
		} else {
			throw new RuntimeException("Table index out of bounds");
		}
	}

}
