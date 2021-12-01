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

	// TODO consider making this take the string, and using ofnullable to make it
	// more flexible on input, while still treating the data the same
	public TrackingTransactionEntry withTrackingAccount(Optional<String> trackingAccount) {
		var retval = clone();
		retval.trackingAccount = trackingAccount;
		return retval;
	}

	@Override
	public TrackingTransactionEntry withAmount(Amount amount) {
		return (TrackingTransactionEntry) super.withAmount(amount);
	}

	@Override
	void balance(BalanceResult soFar) throws TransactionException {
		if (amount() == null) {
			if (soFar.emptyEntry.isEmpty()) {
				soFar.emptyEntry = Optional.of(this);
			} else {
				throw TransactionException.forMultipleEmptyEntries(this);
			}
		} else {
			soFar.categoryTotal = soFar.categoryTotal.add(amount());
		}
	}

	@Override
	Amount missingAmount(BalanceResult balanceResult) {
		return balanceResult.accountTotal.add(balanceResult.categoryTotal.negate());
	}

	@Override
	public TrackingTransactionEntry clone() {
		return (TrackingTransactionEntry) super.clone();
	}

}
