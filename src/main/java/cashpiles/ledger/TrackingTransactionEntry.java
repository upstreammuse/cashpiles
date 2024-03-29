package cashpiles.ledger;

import java.util.Optional;

import cashpiles.currency.Amount;

abstract public class TrackingTransactionEntry extends TransactionEntry {

	private Optional<String> trackingAccount = Optional.empty();

	public TrackingTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	public Amount accountAmount() {
		return amount().negate();
	}

	@Override
	public Transaction.Status accountStatus() {
		return switch (super.accountStatus()) {
		case CLEARED -> Transaction.Status.CLEARED;
		case DISPUTED -> Transaction.Status.DISPUTED;
		case PENDING -> {
			if (trackingAccount.isEmpty()) {
				yield Transaction.Status.CLEARED;
			} else {
				yield Transaction.Status.PENDING;
			}
		}
		};
	}

	@Override
	void addToBalance(Transaction.BalanceResult balancer) throws TransactionException {
		if (amount() == null) {
			if (balancer.emptyEntries < 1) {
				balancer.emptyEntries++;
			} else {
				throw TransactionException.forMultipleEmptyEntries(this);
			}
		} else {
			balancer.categoryTotal = balancer.categoryTotal.add(amount());
		}
	}

	@Override
	public TrackingTransactionEntry clone() {
		return (TrackingTransactionEntry) super.clone();
	}

	@Override
	public int deferral() {
		return 0;
	}

	@Override
	TrackingTransactionEntry fromBalance(Transaction.BalanceResult balancer) {
		if (amount() == null) {
			var retval = withAmount(balancer.accountTotal.add(balancer.categoryTotal.negate()));
			balancer.categoryTotal = balancer.accountTotal;
			return retval;
		} else {
			return this;
		}
	}

	public Optional<String> trackingAccount() {
		return trackingAccount;
	}

	@Override
	public TrackingTransactionEntry withAmount(Amount amount) {
		return (TrackingTransactionEntry) super.withAmount(amount);
	}

	public TrackingTransactionEntry withTrackingAccount(String trackingAccount) {
		var retval = clone();
		retval.trackingAccount = Optional.ofNullable(trackingAccount);
		return retval;
	}

}
