package cashpiles.ui;

import java.util.ArrayList;

import javax.swing.table.AbstractTableModel;

import cashpiles.budget.BudgetPeriod;

@SuppressWarnings("serial")
class BudgetPeriodModel extends AbstractTableModel {

	private static final String[] headers = { "Category", "Type", "Allocation", "Activity", "Balance" };
	private BudgetPeriod period;

	BudgetPeriodModel(BudgetPeriod period) {
		this.period = period;
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
		return period.categories.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		var entries = new ArrayList<>(period.categories.entrySet());
		var entry = entries.get(row);
		return switch (col) {
		case 0 -> entry.getKey();
		case 1 -> entry.getValue().typeString();
		case 2 -> entry.getValue().getAllocation();
		case 3 -> entry.getValue().getActivity();
		case 4 -> entry.getValue().getBalance();
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

	void setPeriod(BudgetPeriod period) {
		this.period = period;
	}

}
