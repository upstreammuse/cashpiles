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

class AccountsWindowController {

	private final Map<String, List<AccountStatement>> accounts = new TreeMap<>();
	private final AccountsTableModel onBudgetModel = new AccountsTableModel();
	private final AccountsTableModel offBudgetModel = new AccountsTableModel();

	void forOnBudgetAccounts(JTable onBudgetAccounts) {
		onBudgetAccounts.setModel(onBudgetModel);
	}

	void forOffBudgetAccounts(JTable offBudgetAccounts) {
		offBudgetAccounts.setModel(offBudgetModel);
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
				onBudgetModel.close(account.name);
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
				offBudgetModel.close(account.name);
			}
			}
		}
		case CLOSED -> {
			switch (account.status) {
			case ON_BUDGET -> {
				var opener = new AccountStatement(Account.Status.ON_BUDGET, new Amount());
				accounts.get(account.name).add(opener);
				onBudgetModel.open(account.name);
			}
			case OFF_BUDGET -> {
				var opener = new AccountStatement(Account.Status.OFF_BUDGET, new Amount());
				accounts.get(account.name).add(opener);
				offBudgetModel.open(account.name);
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

		// TODO this is clumsy
		switch (Lists.lastOf(accounts.get(entry.account)).status) {
		case ON_BUDGET -> {
			Lists.lastOf(accounts.get(entry.account)).add(entry);
			onBudgetModel.add(entry);
		}
		case OFF_BUDGET -> {
			Lists.lastOf(accounts.get(entry.account)).add(entry);
			offBudgetModel.add(entry);
		}
		case CLOSED -> {
			throw AccountException.forClosed(entry);
		}
		}
	}

}
