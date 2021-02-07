package cashpiles.account;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.function.Consumer;

import javax.swing.JTable;

import cashpiles.currency.Amount;
import cashpiles.ledger.Account;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;
import cashpiles.util.Lists;

class AccountsWindowController {

	private final Map<String, List<AccountStatement>> accounts = new TreeMap<>();
	private final AccountsTableModel onBudgetModel = new AccountsTableModel(accounts, Account.Status.ON_BUDGET);
	private final AccountsTableModel offBudgetModel = new AccountsTableModel(accounts, Account.Status.OFF_BUDGET);
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

	void process(Account account) throws AccountException {
		if (!accounts.containsKey(account.name)) {
			accounts.put(account.name, new ArrayList<>());
			accounts.get(account.name).add(new AccountStatement(account.date, Account.Status.CLOSED, new Amount()));
		}

		var latest = Lists.lastOf(accounts.get(account.name));
		switch (latest.status()) {
		case ON_BUDGET -> {
			switch (account.status) {
			case ON_BUDGET -> throw AccountException.forAlreadyOpen(account);
			case OFF_BUDGET -> throw AccountException.forTypeChange(account);
			case CLOSED -> {
				if (!latest.balance().equals(new Amount())) {
					throw AccountException.forNonZeroClose(account);
				}
				var closer = new AccountStatement(latest.endDate(), Account.Status.CLOSED, new Amount());
				accounts.get(account.name).add(closer);
				onBudgetModel.fireTableDataChanged();
			}
			}
		}
		case OFF_BUDGET -> {
			switch (account.status) {
			case ON_BUDGET -> throw AccountException.forTypeChange(account);
			case OFF_BUDGET -> throw AccountException.forAlreadyOpen(account);
			case CLOSED -> {
				if (!latest.balance().equals(new Amount())) {
					throw AccountException.forNonZeroClose(account);
				}
				var closer = new AccountStatement(latest.endDate(), Account.Status.CLOSED, new Amount());
				accounts.get(account.name).add(closer);
				offBudgetModel.fireTableDataChanged();
			}
			}
		}
		case CLOSED -> {
			switch (account.status) {
			case ON_BUDGET -> {
				var opener = new AccountStatement(latest.endDate(), Account.Status.ON_BUDGET, new Amount());
				accounts.get(account.name).add(opener);
				onBudgetModel.fireTableDataChanged();
			}
			case OFF_BUDGET -> {
				var opener = new AccountStatement(latest.endDate(), Account.Status.OFF_BUDGET, new Amount());
				accounts.get(account.name).add(opener);
				offBudgetModel.fireTableDataChanged();
			}
			case CLOSED -> throw AccountException.forAlreadyClosed(account);
			}
		}
		}
	}

	public void process(AccountBalance balance) throws LedgerException {
		if (!accounts.containsKey(balance.account)) {
			throw AccountException.forUnknown(balance);
		}

		var statementList = accounts.get(balance.account);
		var statement = Lists.lastOf(statementList);
		if (!statement.balance().equals(balance.amount)) {
			throw BalanceException.forUnbalanced(balance, statement.balance());
		}

		statementList.add(new AccountStatement(statement.endDate(), statement.status(), statement.balance()));
	}

	public void process(AccountTransactionEntry entry) throws LedgerException {
		if (!accounts.containsKey(entry.account)) {
			throw AccountException.forUnknown(entry);
		}
		Lists.lastOf(accounts.get(entry.account)).add(entry);
		onBudgetModel.fireTableDataChanged();
	}

	public void process(TrackingTransactionEntry entry) throws LedgerException {
		if (!entry.trackingAccount.isPresent()) {
			return;
		}
		if (!accounts.containsKey(entry.trackingAccount.get())) {
			throw AccountException.forUnknown(entry);
		}
		Lists.lastOf(accounts.get(entry.trackingAccount.get())).add(entry);
		offBudgetModel.fireTableDataChanged();
	}

	public void process(UnbalancedTransaction transaction) throws LedgerException {
		if (!accounts.containsKey(transaction.account)) {
			throw AccountException.forUnknown(transaction);
		}
		Lists.lastOf(accounts.get(transaction.account)).add(transaction);
	}

	public void selectOffBudget(int i) {
		var model = new StatementModel(accounts.get(offBudgetModel.getValueAt(i, 0)));
		statementsUI.setModel(model);
		statementsUI.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			transactionsUI.setModel(model.getStatementModel(event));
		});
	}

	public void selectOnBudget(int i) {
		var model = new StatementModel(accounts.get(onBudgetModel.getValueAt(i, 0)));
		statementsUI.setModel(model);
		statementsUI.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			transactionsUI.setModel(model.getStatementModel(event));
		});
	}

}
