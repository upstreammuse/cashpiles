package cashpiles.model2;

import cashpiles.currency.Amount;

class Account extends ModelItem {

	private Amount balance = new Amount();
	private boolean hidden = false;
	private final String name;
	private final boolean onBudget;

	Account(String name, boolean onBudget) {
		this.name = name;
		this.onBudget = onBudget;
	}

	Account asHidden() {
		var account = clone();
		account.hidden = true;
		return account;
	}

	Account asUnhidden() {
		var account = clone();
		account.hidden = false;
		return account;
	}

	boolean hidden() {
		return hidden;
	}

	String name() {
		return name;
	}

	boolean onBudget() {
		return onBudget;
	}

	Account withEntry(AccountTransactionEntry entry) {
		assert (entry.account().equals(name));
		var amount = entry.amount();
		assert (amount.isPresent());
		var account = clone();
		account.balance = account.balance.add(amount.get());
		return account;
	}

	Account withoutEntry(AccountTransactionEntry entry) {
		assert (entry.account().equals(name));
		var amount = entry.amount();
		assert (amount.isPresent());
		var account = clone();
		account.balance = account.balance.add(amount.get().negate());
		return account;
	}

	// @Override
	protected Account clone() {
		return (Account) super.clone();
	}

}
