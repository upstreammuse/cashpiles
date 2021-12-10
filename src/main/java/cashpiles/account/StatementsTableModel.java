package cashpiles.account;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import javax.swing.JTable;
import javax.swing.table.AbstractTableModel;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.model.TransactionParticle;
import cashpiles.util.Lists;

@SuppressWarnings("serial")
class StatementsTableModel extends AbstractTableModel {

	private static final String[] headers = { "Statement Date" };
	private final List<TransactionsTableModel> transactionModels = new ArrayList<>();

	Optional<JTable> transactionsUI = Optional.empty();

	StatementsTableModel() {
		transactionModels.add(new TransactionsTableModel(new Amount()));
	}

	void add(TransactionParticle transaction) {
		Lists.lastOf(transactionModels).add(transaction);
	}

	Amount balance() {
		return Lists.lastOf(transactionModels).balance();
	}

	void forTransactions(JTable transactionsUI) {
		this.transactionsUI = Optional.of(transactionsUI);
		transactionsUI.setModel(new TransactionsTableModel(new Amount()));
	}

	void reconcile(AccountBalance balance) {
		transactionModels.add(Lists.lastOf(transactionModels).reconcile(balance));
	}

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
		case 0 -> transactionModels.get(row).closingDate().map(date -> date.toString())
				.orElse("(Unreconciled transactions)");
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

}
