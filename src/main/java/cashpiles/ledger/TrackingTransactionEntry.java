package cashpiles.ledger;

import java.time.LocalDate;
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
	TrackingTransactionEntry fromBalance(Transaction.BalanceResult balancer) {
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
	public Amount accountAmount() {
		return amount().negate();
	}

	@Override
	public LocalDate date() {
		return parent().date();
	}

	@Override
	public int deferral() {
		return 0;
	}

	@Override
	public String payee() {
		return parent().payee();
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
	public TrackingTransactionEntry clone() {
		return (TrackingTransactionEntry) super.clone();
	}

}
