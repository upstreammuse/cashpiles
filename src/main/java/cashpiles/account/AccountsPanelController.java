package cashpiles.account;

import java.util.Optional;
import java.util.function.Consumer;

import javax.swing.JButton;
import javax.swing.JTable;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountCommand;
import cashpiles.model.Ledger;

class AccountsPanelController {

	private Ledger ledger = new Ledger();
	private Optional<String> selectedAccount = Optional.empty();
	private Optional<Integer> selectedStatement = Optional.empty();
	private Optional<Consumer<Amount>> offBudgetBalanceUI = Optional.empty();
	private Optional<JTable> offBudgetUI = Optional.empty();
	private Optional<Consumer<Amount>> onBudgetBalanceUI = Optional.empty();
	private Optional<JTable> onBudgetUI = Optional.empty();
	private Optional<JTable> statementsUI = Optional.empty();
	private Optional<JTable> transactionsUI = Optional.empty();

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

	void forStatements(JTable statements) {
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
										ledger.getTransactions(accountName, statementIndex))),
								() -> transactions.setModel(new TransactionsTableModel())),
						() -> transactions.setModel(new TransactionsTableModel())));
	}

}
