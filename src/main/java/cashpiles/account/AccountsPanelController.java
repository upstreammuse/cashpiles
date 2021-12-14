package cashpiles.account;

import java.util.Optional;
import java.util.function.Consumer;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JTable;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountCommand;
import cashpiles.model.Ledger;

class AccountsPanelController {

	private Ledger ledger = new Ledger();
	private final JFrame parent;
	private Optional<String> selectedAccount = Optional.empty();
	private Optional<Integer> selectedStatement = Optional.empty();
	private Optional<Consumer<Amount>> offBudgetBalanceUI = Optional.empty();
	private Optional<JTable> offBudgetUI = Optional.empty();
	private Optional<Consumer<Amount>> onBudgetBalanceUI = Optional.empty();
	private Optional<JTable> onBudgetUI = Optional.empty();
	private Optional<JTable> statementsUI = Optional.empty();
	private Optional<JTable> transactionsUI = Optional.empty();

	AccountsPanelController(JFrame parent) {
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
				var statements = ledger.getStatements(accountName);

				// TODO get last statement from the statements list for the given account, and
				// give that statement to the dialog to break into a reconciled part and an
				// unreconciled part by clearing the transactions that belong to the reconciled
				// part, and excluding/deferring the transactions that belong in the remainder
				var dialog = new ReconciliationDialog(parent, statements);
				dialog.setVisible(true);

				// TODO for each transaction that the dialog worked on...
				// - update status to

				// TODO this view/dialog works on particles, but need to modify whole
				// transactions in the ledger, so...
				// - need a link from a particle back to its transaction
				// - need the ability to replace items in the ledger (or at least transactions)
				// - when doing this, does the ledger accept a modified transaction and then
				// figure out which particles to change?
				// - or does the ledger accept a single particle that modifies a single
				// transaction? -> this is more doable, assuming each particle and each xact
				// have IDs that can be used to replace them in-situ
				// - it isn't, since a particle doesn't point to a transaction entry. if it
				// does, then we can do it, because a particle = entry -> transaction

				// TODO so the reconcile dialog gets a set of particles and...
				// - for each particle after a date, ignore it, since balance command will also
				// ignore it
				// - for each particle <= date, give user ability to see its current status
				// (pending/cleared/disputed)
				// - for each particle <= date, give user ability to change status
				// (reconciled/notreconciled)
				// - for each reconciled particle, set its status to cleared if its status is
				// pending
				// - for each notreconciled particle, set its status to deferred (increment its
				// deferral count?)
				// then once the dialog returns...
				// - for each particle, modify it in the ledger -> modify xact entry -> modify
				// xact
				// - instead of a general item modification, this can be a structured command to
				// set the status of an item ID, and it can throw errors if you pass an ID of
				// something that doesn't have .... no, this requires the ledger to discriminate
				// types, which it doesn't do
				// - create a balance command for account/amount/date
				// - add balance command to ledger

				// TODO process the reconciliation results
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
