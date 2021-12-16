package cashpiles.budget.redo;

import javax.swing.table.AbstractTableModel;

import cashpiles.model.CategoriesView;

@SuppressWarnings("serial")
class CategoryTableModel extends AbstractTableModel {

	private static final String[] headers = { "Category", "Type", "Owner", "Balance" };

	private final CategoriesView view;

	CategoryTableModel(CategoriesView view) {
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
		return view.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		var category = view.get(row);
		return switch (col) {
		case 0 -> view.name(row);
		case 1 -> category.type();
		case 2 -> category.owner();
		case 3 -> category.balance();
		default -> throw new IllegalArgumentException();
		};
	}

}
