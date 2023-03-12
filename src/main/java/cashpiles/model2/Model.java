package cashpiles.model2;

import java.util.Map;
import java.util.TreeMap;

class Model extends ModelBase {

	private Map<String, Account> accounts = new TreeMap<>();

	Model hideAccount(String name) throws ModelException {
		if (!accounts.containsKey(name)) {
			throw ModelException.accountNotExist(name);
		}
		if (accounts.get(name).hidden()) {
			throw ModelException.accountNotOpen(name);
		}

		var model = clone();
		model.accounts.put(name, model.accounts.get(name).asHidden());
		return model;
	}

	Model openAccount(String name, boolean onBudget) throws ModelException {
		var exists = accounts.containsKey(name);
		if (exists) {
			if (!accounts.get(name).hidden()) {
				throw ModelException.accountAlreadyOpen(name);
			}
			if (accounts.get(name).onBudget() != onBudget) {
				throw ModelException.accountTypeMismatch(name);
			}
		}

		var model = clone();
		if (exists) {
			model.accounts.put(name, model.accounts.get(name).asUnhidden());
		} else {
			model.accounts.put(name, new Account(name, onBudget));
		}
		return model;
	}

	// @Override
	protected Model clone() {
		var model = (Model) super.clone();
		model.accounts = new TreeMap<>(accounts);
		return model;
	}

}
