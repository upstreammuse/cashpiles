package cashpiles.budget.redo;

import java.util.Optional;

import javax.swing.table.AbstractTableModel;

import cashpiles.model.CategoryView;

@SuppressWarnings("serial")
class PeriodTableModel extends AbstractTableModel {

	public static final String[] headers = { "Start Date", "End Date", "Allocation", "Activity", "Balance" };

	private final Optional<CategoryView> view;

	PeriodTableModel() {
		view = Optional.empty();
	}

	PeriodTableModel(CategoryView view) {
		this.view = Optional.of(view);
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
		return view.map(view -> view.size()).orElse(0);
	}

	@Override
	public Object getValueAt(int row, int col) {
		var period = view.orElseThrow().get(row);
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
