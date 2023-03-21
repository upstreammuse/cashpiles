package cashpiles.model2;

import java.util.Optional;

class AccountTransactionEntry extends TransactionEntry {

	private String account;
	private Optional<Transaction.Status> status = Optional.empty();

	AccountTransactionEntry(String account) {
		this.account = account;
	}

	String account() {
		return account;
	}

	@Override
	void addToBalance(TransactionBalancer balancer) throws ModelException {
		if (amount().isPresent()) {
			balancer.accountBalance = balancer.accountBalance.add(amount().get());
		} else {
			if (balancer.blankEntry) {
				throw ModelException.multipleBlankAmounts();
			} else {
				balancer.blankEntry = true;
			}
		}
	}

	@Override
	Model addToModel(Model model) throws ModelException {
		return model.withTransactionEntry(this);
	}

	@Override
	TransactionEntry getFromBalance(TransactionBalancer balancer) {
		if (amount().isEmpty()) {
			var entry = withAmount(balancer.categoryBalance.add(balancer.accountBalance.negate()));
			return entry;
		} else {
			return this;
		}
	}

	@Override
	Model removeFromModel(Model model) throws ModelException {
		return model.withoutTransactionEntry(this);
	}

	@Override
	Optional<Transaction.Status> status() {
		return status;
	}

	@Override
	AccountTransactionEntry withMergedStatus(Transaction.Status status) {
		var entry = clone();
		entry.status = Optional.of(entry.status.orElse(status));
		return entry;
	}

	@Override
	protected AccountTransactionEntry clone() {
		return (AccountTransactionEntry) super.clone();
	}

}
