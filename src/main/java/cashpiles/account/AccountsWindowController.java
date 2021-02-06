package cashpiles.account;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.function.Consumer;

import javax.swing.JTable;

import cashpiles.currency.Amount;
import cashpiles.ledger.Account;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.util.Lists;

// TODO this might be doable as separate pieces
class AccountsWindowController {

	private final Map<String, List<AccountStatement>> accounts = new TreeMap<>();
	private final TransactionTableModel allTransactions = new TransactionTableModel(accounts);
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
		transactions.setModel(allTransactions);
	}

	void onOnBudgetBalance(Consumer<Amount> consumer) {
		// TODO Auto-generated method stub
	}

	void onOffBudgetBalance(Consumer<Amount> consumer) {
		// TODO Auto-generated method stub
	}

	void process(Account account) throws AccountException {
		if (!accounts.containsKey(account.name)) {
			accounts.put(account.name, new ArrayList<>());
			accounts.get(account.name).add(new AccountStatement(Account.Status.CLOSED, new Amount()));
		}

		var latest = Lists.lastOf(accounts.get(account.name));
		switch (latest.status) {
		case ON_BUDGET -> {
			switch (account.status) {
			case ON_BUDGET -> throw AccountException.forAlreadyOpen(account);
			case OFF_BUDGET -> throw AccountException.forTypeChange(account);
			case CLOSED -> {
				if (!latest.balance().equals(new Amount())) {
					throw AccountException.forNonZeroClose(account);
				}
				var closer = new AccountStatement(Account.Status.CLOSED, new Amount());
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
				var closer = new AccountStatement(Account.Status.CLOSED, new Amount());
				accounts.get(account.name).add(closer);
				offBudgetModel.fireTableDataChanged();
			}
			}
		}
		case CLOSED -> {
			switch (account.status) {
			case ON_BUDGET -> {
				var opener = new AccountStatement(Account.Status.ON_BUDGET, new Amount());
				accounts.get(account.name).add(opener);
				onBudgetModel.fireTableDataChanged();
			}
			case OFF_BUDGET -> {
				var opener = new AccountStatement(Account.Status.OFF_BUDGET, new Amount());
				accounts.get(account.name).add(opener);
				offBudgetModel.fireTableDataChanged();
			}
			case CLOSED -> throw AccountException.forAlreadyClosed(account);
			}
		}
		}
	}

	public void process(AccountTransactionEntry entry) throws AccountException {
		if (!accounts.containsKey(entry.account)) {
			throw AccountException.forUnknown(entry);
		}

		switch (Lists.lastOf(accounts.get(entry.account)).status) {
		case ON_BUDGET -> {
			Lists.lastOf(accounts.get(entry.account)).add(entry);
			onBudgetModel.fireTableDataChanged();
			allTransactions.fireTableDataChanged();
		}
		case OFF_BUDGET -> {
			Lists.lastOf(accounts.get(entry.account)).add(entry);
			offBudgetModel.fireTableDataChanged();
			allTransactions.fireTableDataChanged();
		}
		case CLOSED -> {
			throw AccountException.forClosed(entry);
		}
		}
	}

}
