package cashpiles.account;

import java.util.Map;
import java.util.Optional;
import java.util.TreeMap;
import java.util.function.Consumer;

import javax.swing.JTable;
import javax.swing.ListSelectionModel;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;

class AccountsWindowController {

	private final Map<String, Account> accounts = new TreeMap<>();
	private final AccountsTableModel onBudgetModel = new AccountsTableModel(accounts, AccountCommand.Status.ON_BUDGET);
	private final AccountsTableModel offBudgetModel = new AccountsTableModel(accounts,
			AccountCommand.Status.OFF_BUDGET);
	private Optional<String> selectedAccount = Optional.empty();
	private JTable statementsUI;
	private JTable transactionsUI;

	void forOnBudgetAccounts(JTable onBudgetAccounts) {
		onBudgetAccounts.setModel(onBudgetModel);
	}

	void forOffBudgetAccounts(JTable offBudgetAccounts) {
		offBudgetAccounts.setModel(offBudgetModel);
	}

	void forStatements(JTable statements) {
		statementsUI = statements;
		statements.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		// TODO does this have to be re-added when the model changes, i.e. when we
		// select a new account?
		statements.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			selectedAccount.ifPresent(account -> {
				for (int i = event.getFirstIndex(); i <= event.getLastIndex(); i++) {
					if (statements.isRowSelected(i)) {
						accounts.get(account).selectStatement(i);
					}
				}
			});
		});
	}

	void forTransactions(JTable transactions) {
		transactionsUI = transactions;
	}

	void onOnBudgetBalance(Consumer<Amount> consumer) {
		onBudgetModel.addTableModelListener(event -> {
			consumer.accept(onBudgetModel.balance());
		});
	}

	void onOffBudgetBalance(Consumer<Amount> consumer) {
		offBudgetModel.addTableModelListener(event -> {
			consumer.accept(offBudgetModel.balance());
		});
	}

	void process(AccountCommand command) throws AccountException {
		if (!accounts.containsKey(command.name)) {
			accounts.put(command.name, new Account());
		}
		accounts.get(command.name).update(command);
		onBudgetModel.fireTableDataChanged();
		offBudgetModel.fireTableDataChanged();
	}

	public void process(AccountBalance balance) throws LedgerException {
		if (!accounts.containsKey(balance.account())) {
			throw AccountException.forUnknown(balance);
		}
		accounts.get(balance.account()).closeStatement(balance);
		onBudgetModel.fireTableDataChanged();
		offBudgetModel.fireTableDataChanged();
	}

	public void process(AccountTransactionEntry entry) throws LedgerException {
		if (!accounts.containsKey(entry.account)) {
			throw AccountException.forUnknown(entry);
		}
		accounts.get(entry.account).add(entry);
		onBudgetModel.fireTableDataChanged();
	}

	// TODO it might be possible to do this without isPresent/get, but don't need to
	// solve that right now
	public void process(TrackingTransactionEntry entry) throws LedgerException {
		if (!entry.trackingAccount.isPresent()) {
			return;
		}
		if (!accounts.containsKey(entry.trackingAccount.get())) {
			throw AccountException.forUnknown(entry);
		}
		accounts.get(entry.trackingAccount.get()).add(entry);
		offBudgetModel.fireTableDataChanged();
	}

	public void process(UnbalancedTransaction transaction) throws LedgerException {
		if (!accounts.containsKey(transaction.account)) {
			throw AccountException.forUnknown(transaction);
		}
		accounts.get(transaction.account).add(transaction);
		offBudgetModel.fireTableDataChanged();
	}

	public void selectOffBudget(int i) {
		selectedAccount = Optional.of((String) offBudgetModel.getValueAt(i, 0));
		var account = accounts.get(selectedAccount.get());
		account.forTransactions(transactionsUI);
		statementsUI.setModel(account);
	}

	public void selectOnBudget(int i) {
		selectedAccount = Optional.of((String) onBudgetModel.getValueAt(i, 0));
		var account = accounts.get(selectedAccount.get());
		account.forTransactions(transactionsUI);
		statementsUI.setModel(account);
	}

}
