package cashpiles.account;

import java.awt.event.ActionEvent;
import java.util.Optional;
import java.util.function.Consumer;

import javax.swing.JTable;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.Transaction;
import cashpiles.ledger.UnbalancedTransaction;
import cashpiles.model.Ledger;
import cashpiles.model.TransactionParticle;

class AccountsPanelController implements ItemProcessor {

	private final AccountsTableModel offBudgetModel = new AccountsTableModel();
	private final AccountsTableModel onBudgetModel = new AccountsTableModel();
	private Optional<String> selectedAccount = Optional.empty();
	private JTable statementsUI;
	private JTable transactionsUI;

	void forAccounts(JTable onBudgetAccounts, JTable offBudgetAccounts) {
		onBudgetAccounts.setModel(onBudgetModel);
		offBudgetAccounts.setModel(offBudgetModel);
		offBudgetAccounts.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			for (int i = event.getFirstIndex(); i <= event.getLastIndex(); i++) {
				if (offBudgetAccounts.isRowSelected(i)) {
					onBudgetAccounts.clearSelection();
					selectOffBudget(i);
				}
			}
		});
		onBudgetAccounts.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			for (int i = event.getFirstIndex(); i <= event.getLastIndex(); i++) {
				if (onBudgetAccounts.isRowSelected(i)) {
					offBudgetAccounts.clearSelection();
					selectOnBudget(i);
				}
			}
		});
	}

	void forOffBudgetBalance(Consumer<Amount> consumer) {
		offBudgetModel.addTableModelListener(event -> {
			consumer.accept(offBudgetModel.balance());
		});
	}

	void forOnBudgetBalance(Consumer<Amount> consumer) {
		onBudgetModel.addTableModelListener(event -> {
			consumer.accept(onBudgetModel.balance());
		});
	}

	void forStatements(JTable statements) {
		statementsUI = statements;
		statements.setModel(new StatementsTableModel());
		statements.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			selectedAccount.ifPresent(account -> {
				for (int i = event.getFirstIndex(); i <= event.getLastIndex(); i++) {
					if (statements.isRowSelected(i)) {
						var statementModel = onBudgetModel.statements.get(account);
						if (statementModel == null) {
							statementModel = offBudgetModel.statements.get(account);
						}
						statementModel.selectStatement(i);
					}
				}
			});
		});
	}

	void forTransactions(JTable transactions) {
		transactionsUI = transactions;
		transactions.setModel(new TransactionsTableModel(new Amount()));
	}

	void setLedger(Ledger ledger) {
		ledger.addListener(this::refresh);
		refresh(ledger);
	}

	private void selectOffBudget(int i) {
		selectedAccount = Optional.of((String) offBudgetModel.getValueAt(i, 0));
		var statementModel = offBudgetModel.statements.get(selectedAccount.get());
		statementModel.forTransactions(transactionsUI);
		statementsUI.setModel(statementModel);
	}

	private void selectOnBudget(int i) {
		selectedAccount = Optional.of((String) onBudgetModel.getValueAt(i, 0));
		var statementModel = onBudgetModel.statements.get(selectedAccount.get());
		statementModel.forTransactions(transactionsUI);
		statementsUI.setModel(statementModel);
	}

	private void refresh(ActionEvent action) {
		refresh((Ledger) action.getSource());
	}

	private void refresh(Ledger ledger) {
		offBudgetModel.statements.clear();
		onBudgetModel.statements.clear();
		selectedAccount = Optional.empty();
		ledger.process(this);
	}

	@Override
	public void finish() {
		offBudgetModel.fireTableDataChanged();
		onBudgetModel.fireTableDataChanged();
	}

	@Override
	public void process(AccountBalance balance) {
		var statements = onBudgetModel.statements.get(balance.account());
		if (statements == null) {
			statements = offBudgetModel.statements.get(balance.account());
		}
		statements.reconcile(balance);
	}

	@Override
	public void process(AccountCommand command) {
		switch (command.status()) {
		case ON_BUDGET -> onBudgetModel.statements.put(command.account(), new StatementsTableModel());
		case OFF_BUDGET -> offBudgetModel.statements.put(command.account(), new StatementsTableModel());
		case CLOSED -> {
			onBudgetModel.statements.remove(command.account());
			offBudgetModel.statements.remove(command.account());
		}
		}
	}

	@Override
	public void process(AccountTransactionEntry entry) {
		var statements = onBudgetModel.statements.get(entry.account());
		var dXact = new TransactionParticle().withAmount(entry.amount()).withDate(entry.parent().date())
				.withPayee(entry.parent().payee()).withStatus(entry.parent().status());
		statements.add(dXact);
	}

	@Override
	public boolean process(Transaction transaction) {
		return true;
	}

	@Override
	public void process(CategoryTransactionEntry entry) {
		processTracking(entry);
	}

	@Override
	public void process(OwnerTransactionEntry entry) {
		processTracking(entry);
	}

	@Override
	public void process(UnbalancedTransaction transaction) {
		var statements = offBudgetModel.statements.get(transaction.account());
		var dXact = new TransactionParticle().withAmount(transaction.amount()).withDate(transaction.date())
				.withPayee(transaction.payee()).withStatus(transaction.status());
		statements.add(dXact);
	}

	private void processTracking(TrackingTransactionEntry entry) {
		entry.trackingAccount().ifPresent(account -> {
			var statements = offBudgetModel.statements.get(account);
			var dXact = new TransactionParticle().withAmount(entry.amount()).withDate(entry.parent().date())
					.withPayee(entry.parent().payee()).withStatus(entry.parent().status());
			statements.add(dXact);
		});
	}

}
