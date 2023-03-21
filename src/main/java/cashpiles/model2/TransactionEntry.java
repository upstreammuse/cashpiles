package cashpiles.model2;

import java.util.Optional;

import cashpiles.currency.Amount;

abstract class TransactionEntry extends ModelItem {

	private Optional<Amount> amount = Optional.empty();
	private Transaction parent;

	abstract void addToBalance(TransactionBalancer balancer) throws ModelException;

	abstract Model addToModel(Model m) throws ModelException;

	Optional<Amount> amount() {
		return amount;
	}

	abstract TransactionEntry getFromBalance(TransactionBalancer balancer);

	Transaction parent() {
		return parent;
	}

	abstract Model removeFromModel(Model m) throws ModelException;

	abstract Optional<Transaction.Status> status();

	TransactionEntry withAmount(Amount amount) {
		var entry = clone();
		entry.amount = Optional.of(amount);
		return entry;
	}

	abstract TransactionEntry withMergedStatus(Transaction.Status status);

	TransactionEntry withParent(Transaction transaction) {
		var entry = clone();
		entry.parent = transaction;
		return entry;
	}

	@Override
	protected TransactionEntry clone() {
		var entry = (TransactionEntry) super.clone();
		entry.parent = null;
		return entry;
	}

}
