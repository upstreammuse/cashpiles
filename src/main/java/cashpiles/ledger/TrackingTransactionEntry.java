package cashpiles.ledger;

import java.util.Optional;

import cashpiles.currency.Amount;

abstract public class TrackingTransactionEntry extends TransactionEntry {

	private Optional<String> trackingAccount = Optional.empty();

	public TrackingTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public Optional<String> trackingAccount() {
		return trackingAccount;
	}

	public TrackingTransactionEntry withTrackingAccount(String trackingAccount) {
		var retval = clone();
		retval.trackingAccount = Optional.ofNullable(trackingAccount);
		return retval;
	}

	@Override
	void addToBalance(BalanceResult balancer) throws TransactionException {
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
	TrackingTransactionEntry fromBalance(BalanceResult balancer) {
		if (amount() == null) {
			var retval = withAmount(balancer.accountTotal.add(balancer.categoryTotal.negate()));
			balancer.categoryTotal = balancer.accountTotal;
			return retval;
		} else {
			return this;
		}
	}

	@Override
	public TrackingTransactionEntry withAmount(Amount amount) {
		return (TrackingTransactionEntry) super.withAmount(amount);
	}

	@Override
	public TrackingTransactionEntry clone() {
		return (TrackingTransactionEntry) super.clone();
	}

}
