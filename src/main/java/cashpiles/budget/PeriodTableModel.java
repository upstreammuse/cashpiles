package cashpiles.budget;

import javax.swing.table.AbstractTableModel;

import cashpiles.model2.PeriodsView;

@SuppressWarnings("serial")
class PeriodTableModel extends AbstractTableModel {

	private static final String[] headers = { "Start Date", "End Date", "Allocation", "Activity", "Balance" };

	private final PeriodsView view;

	PeriodTableModel(PeriodsView view) {
		this.view = view;
	}

	@Override
	public int getColumnCount() {
		return headers.length;
	}

	@Override
	public String getColumnName(int col) {
		return headers[col];
	}

	@Override
	public int getRowCount() {
		return view.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		var period = view.get(row);
		return switch (col) {
		case 0 -> period.dates().startDate();
		case 1 -> period.dates().endDate();
		case 2 -> period.allocation();
		case 3 -> period.activity();
		case 4 -> period.balance();
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

}
