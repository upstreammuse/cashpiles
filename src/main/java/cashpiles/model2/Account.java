package cashpiles.model2;

class Account extends ModelItem {

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

	// @Override
	protected Account clone() {
		return (Account) super.clone();
	}

}
