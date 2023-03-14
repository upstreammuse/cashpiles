package cashpiles.model2;

class AccountTransactionEntry extends TransactionEntry {

	private String account;

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
	Model removeFromModel(Model m) {
		// TODO Auto-generated method stub
		return null;
	}

}
