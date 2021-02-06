package cashpiles.account;

import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.Account;
import cashpiles.ledger.AccountTransactionEntry;

public class AccountStatement {

	Amount startBalance;
	Account.Status status;
	List<AccountTransactionEntry> transactions = new ArrayList<>();

	AccountStatement(Account.Status status, Amount startBalance) {
		this.status = status;
		this.startBalance = startBalance;
	}

	public void add(AccountTransactionEntry entry) {
		transactions.add(entry);
	}

	public Amount balance() {
		return transactions.stream().map(transaction -> transaction.amount).reduce(new Amount(),
				(total, amount) -> total.add(amount));
	}

}
