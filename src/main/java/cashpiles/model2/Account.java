package cashpiles.model2;

class Account extends ModelBase {

	private boolean hidden = false;
	private final String name;
	private final boolean onBudget;

	Account(String name, boolean onBudget) {
		this.name = name;
		this.onBudget = onBudget;
	}

	boolean hidden() {
		return hidden;
	}

	Account hide() {
		var account = clone();
		account.hidden = true;
		return account;
	}

	String name() {
		return name;
	}

	boolean onBudget() {
		return onBudget;
	}

	Account unhide() {
		var account = clone();
		account.hidden = false;
		return account;
	}

	// @Override
	protected Account clone() {
		return (Account) super.clone();
	}

}
