package cashpiles.budget;

import java.util.Optional;

import javax.swing.table.AbstractTableModel;

import cashpiles.model2.CategoriesView;

@SuppressWarnings("serial")
class CategoryTableModel extends AbstractTableModel {

	private static final String[] headers = { "Category", "Type", "Owner", "Allocation", "Activity", "Balance" };

	private final Optional<CategoriesView> view;

	CategoryTableModel() {
		view = Optional.empty();
	}

	CategoryTableModel(CategoriesView categories) {
		view = Optional.of(categories);
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
		var v = view.orElseThrow(() -> new IllegalArgumentException("Unexpected row value: " + row));
		var name = v.names().get(row);
		var category = v.get(name);
		return switch (col) {
		case 0 -> name;
		case 1 -> category.type();
		case 2 -> category.owner();
		case 3 -> category.allocation();
		case 4 -> category.activity();
		case 5 -> category.balance();
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

}
