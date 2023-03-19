package cashpiles.model2;

import javax.swing.table.AbstractTableModel;

@SuppressWarnings("serial")
class AccountsTableModel extends AbstractTableModel {

	private static final String[] headers = { "Account", "Cleared Balance", "Uncleared Balance" };

	private final Model model;
	private final boolean onBudget;

	AccountsTableModel(Model model, boolean onBudget) {
		this.model = model;
		this.onBudget = onBudget;
		;
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
		return model.accountNames(onBudget).size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		try {
			var names = model.accountNames(onBudget);
			return switch (col) {
			case 0 -> names.get(row);
			case 1 -> model.accountBalance(names.get(row), true);
			case 2 -> model.accountBalance(names.get(row), false);
			default -> throw new IllegalArgumentException();
			};
		} catch (ModelException ex) {
			return "Internal Error!";
		}
	}

}
