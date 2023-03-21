package cashpiles.model2;

import java.util.Optional;

class CategoryTransactionEntry extends TransactionEntry {

	private String category;

	CategoryTransactionEntry(String category) {
		this.category = category;
	}

	@Override
	void addToBalance(TransactionBalancer balancer) throws ModelException {
		if (amount().isPresent()) {
			balancer.categoryBalance = balancer.categoryBalance.add(amount().get());
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

	String category() {
		return category;
	}

	@Override
	TransactionEntry getFromBalance(TransactionBalancer balancer) {
		if (amount().isEmpty()) {
			var entry = withAmount(balancer.accountBalance.add(balancer.categoryBalance.negate()));
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
		return Optional.of(Transaction.Status.CLEARED);
	}

	@Override
	CategoryTransactionEntry withMergedStatus(Transaction.Status status) {
		return this;
	}

}
