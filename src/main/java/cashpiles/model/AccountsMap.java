package cashpiles.model;

import java.util.TreeMap;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountCommand.Status;

@SuppressWarnings("serial")
class AccountsMap extends TreeMap<String, Account> implements AccountsView {

	@Override
	public Amount balance(Status status) {
		return entrySet().parallelStream().map(entry -> entry.getValue()).filter(account -> account.status() == status)
				.map(account -> account.balance()).reduce(new Amount(), (total, balance) -> total.add(balance));
	}

	@Override
	public Amount balance(Status status, int index) {
		return entrySet().stream().map(entry -> entry.getValue()).filter(account -> account.status() == status)
				.map(account -> account.balance()).skip(index).findFirst().get();
	}

	@Override
	public int size(Status status) {
		return Math.toIntExact(entrySet().parallelStream().map(entry -> entry.getValue())
				.filter(account -> account.status() == status).count());
	}

	@Override
	public String name(Status status, int index) {
		return entrySet().stream().filter(entry -> entry.getValue().status() == status).map(entry -> entry.getKey())
				.skip(index).findFirst().get();
	}

}
