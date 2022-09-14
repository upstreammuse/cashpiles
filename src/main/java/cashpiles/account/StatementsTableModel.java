package cashpiles.account;

import java.util.Optional;

import javax.swing.table.AbstractTableModel;

import cashpiles.model.StatementsView;

@SuppressWarnings("serial")
class StatementsTableModel extends AbstractTableModel {

	private static final String[] headers = { "Statement Date" };

	private transient final Optional<StatementsView> view;

	StatementsTableModel() {
		this.view = Optional.empty();
	}

	StatementsTableModel(StatementsView view) {
		this.view = Optional.of(view);
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
		return view.map(view -> view.size()).orElse(0);
	}

	@Override
	public Object getValueAt(int row, int col) {
		return switch (col) {
		case 0 -> view.orElseThrow().get(row).closingDate().map(date -> date.toString())
				.orElse("(Unreconciled transactions)");
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

}
