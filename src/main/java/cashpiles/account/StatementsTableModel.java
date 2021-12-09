package cashpiles.account;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import javax.swing.JTable;
import javax.swing.table.AbstractTableModel;

import cashpiles.currency.Amount;

@SuppressWarnings("serial")
class StatementsTableModel extends AbstractTableModel {

	private static final String[] headers = { "Statement Date" };
	final List<TransactionsTableModel> transactionModels = new ArrayList<>();

	// TODO maybe move this to the overall controller
	Optional<JTable> transactionsUI = Optional.empty();

	StatementsTableModel() {
		transactionModels.add(new TransactionsTableModel(new Amount()));
	}

	// TODO not sure if this should clear the model for this table when it is set up
	// initially?
	void forTransactions(JTable transactionsUI) {
		this.transactionsUI = Optional.of(transactionsUI);
	}

	// TODO maybe move this to the overall controller to match most of the other
	// interaction linkage
	void selectStatement(int i) {
		transactionsUI.ifPresent(ui -> {
			ui.setModel(transactionModels.get(i));
		});
	}

	@Override
	public void fireTableDataChanged() {
		super.fireTableDataChanged();
		transactionModels.forEach(model -> model.fireTableDataChanged());
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
		return transactionModels.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		return switch (col) {
		case 0 -> transactionModels.get(row).endDate.isPresent() ? transactionModels.get(row).endDate.get()
				: "(Unreconciled transactions)";
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

}
