package cashpiles.account;

import java.util.Optional;
import java.util.function.Consumer;

import javax.swing.JButton;
import javax.swing.JOptionPane;
import javax.swing.JTable;

import cashpiles.MainWindow;
import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.LedgerException;
import cashpiles.model.Ledger;

class AccountsPanelController {

	private Ledger ledger = new Ledger();
	private final MainWindow parent;
	private Optional<String> selectedAccount = Optional.empty();
	private Optional<Integer> selectedStatement = Optional.empty();
	private Optional<Consumer<Amount>> offBudgetBalanceUI = Optional.empty();
	private Optional<JTable> offBudgetUI = Optional.empty();
	private Optional<Consumer<Amount>> onBudgetBalanceUI = Optional.empty();
	private Optional<JTable> onBudgetUI = Optional.empty();
	private Optional<JTable> statementsUI = Optional.empty();
	private Optional<JTable> transactionsUI = Optional.empty();

	AccountsPanelController(MainWindow parent) {
		this.parent = parent;
	}

	void forAccounts(JTable onBudgetAccounts, JTable offBudgetAccounts) {
		onBudgetUI = Optional.of(onBudgetAccounts);
		offBudgetUI = Optional.of(offBudgetAccounts);
		offBudgetAccounts.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			for (int i = event.getFirstIndex(); i <= event.getLastIndex(); i++) {
				if (offBudgetAccounts.isRowSelected(i)) {
					onBudgetAccounts.clearSelection();
					selectedAccount = Optional.of((String) offBudgetAccounts.getValueAt(i, 0));
					refreshStatements();
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
					selectedAccount = Optional.of((String) onBudgetAccounts.getValueAt(i, 0));
					refreshStatements();
				}
			}
		});
		refresh();
	}

	void forNewTransactions(JButton newTransactions) {
		// TODO add transaction to ledger
	}

	void forOffBudgetBalance(Consumer<Amount> consumer) {
		offBudgetBalanceUI = Optional.of(consumer);
		refreshBalances();
	}

	void forOnBudgetBalance(Consumer<Amount> consumer) {
		onBudgetBalanceUI = Optional.of(consumer);
		refreshBalances();
	}

	public void forReconcile(JButton reconcileButton) {
		reconcileButton.addActionListener(action -> {
			selectedAccount.ifPresentOrElse(accountName -> {
				var dialog = new ReconciliationDialog(parent, ledger.getStatements(accountName));
				dialog.setVisible(true);
				if (!dialog.ok()) {
					return;
				}

				var modifier = new LedgerReconciler();
				for (var item : dialog.selected()) {
					modifier = modifier.withClearedTransaction(item);
				}
				for (var item : dialog.unselected()) {
					modifier = modifier.withDeferredTransaction(item);
				}
				modifier = modifier.withStatement(new AccountBalance("", 0, "").withAccount(accountName)
						.withAmount(dialog.selectedTotal()).withDate(dialog.date()));
				ledger.process(modifier);

				try {
					parent.setLedger(modifier.toLedger());
				} catch (LedgerException ex) {
					JOptionPane.showMessageDialog(parent, "Error while reconciling.  " + ex.getLocalizedMessage(),
							"Reconciliation Error", JOptionPane.ERROR_MESSAGE);
				}
			}, () -> {
				JOptionPane.showMessageDialog(parent, "No account was selected.", "Reconciliation Error",
						JOptionPane.ERROR_MESSAGE);
			});
		});
	}

	void forStatements(JTable statements) {
		statementsUI = Optional.of(statements);
		statements.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			selectedAccount.ifPresent(accountName -> transactionsUI.ifPresent(transactions -> {
				for (int i = event.getFirstIndex(); i <= event.getLastIndex(); i++) {
					if (statements.isRowSelected(i)) {
						selectedStatement = Optional.of(i);
						refreshTransactions();
					}
				}
			}));
		});
		refreshStatements();
	}

	void forTransactions(JTable transactions) {
		transactionsUI = Optional.of(transactions);
		refreshTransactions();
	}

	void setLedger(Ledger ledger) {
		this.ledger = ledger;
		ledger.addListener(action -> refresh());
		refresh();
	}

	private void refresh() {
		selectedAccount = Optional.empty();
		offBudgetUI.ifPresent(offBudget -> offBudget
				.setModel(new AccountsTableModel(ledger.getAccounts(), AccountCommand.Status.OFF_BUDGET)));
		onBudgetUI.ifPresent(onBudget -> onBudget
				.setModel(new AccountsTableModel(ledger.getAccounts(), AccountCommand.Status.ON_BUDGET)));
		refreshBalances();
		refreshStatements();
	}

	private void refreshBalances() {
		offBudgetBalanceUI.ifPresent(
				offBudget -> offBudget.accept(ledger.getAccounts().balance(AccountCommand.Status.OFF_BUDGET)));
		onBudgetBalanceUI
				.ifPresent(onBudget -> onBudget.accept(ledger.getAccounts().balance(AccountCommand.Status.ON_BUDGET)));
	}

	private void refreshStatements() {
		selectedStatement = Optional.empty();
		statementsUI.ifPresent(statements -> selectedAccount.ifPresentOrElse(
				accountName -> statements.setModel(new StatementsTableModel(ledger.getStatements(accountName))),
				() -> statements.setModel(new StatementsTableModel())));
		refreshTransactions();
	}

	private void refreshTransactions() {
		transactionsUI
				.ifPresent(transactions -> selectedAccount.ifPresentOrElse(
						accountName -> selectedStatement.ifPresentOrElse(
								statementIndex -> transactions.setModel(new TransactionsTableModel(
										ledger.getStatements(accountName).get(statementIndex))),
								() -> transactions.setModel(new TransactionsTableModel())),
						() -> transactions.setModel(new TransactionsTableModel())));
	}

}
