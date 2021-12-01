package cashpiles.ledger;

import java.util.Optional;

import cashpiles.currency.Amount;

// TODO I think Cloneable is pretty much exactly what I need for this use case, but if it falls apart, I can use the duplicate() construction like I did with Budget entries.  If it works here, refactor the other ones so they match
abstract public class TrackingTransactionEntry extends TransactionEntry {

	private Optional<String> trackingAccount = Optional.empty();

	public TrackingTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public TrackingTransactionEntry(TrackingTransactionEntry other) {
		super(other);
		trackingAccount = other.trackingAccount;
	}

	public Optional<String> trackingAccount() {
		return trackingAccount;
	}

	@Override
	public TrackingTransactionEntry withAmount(Amount amount) {
		return (TrackingTransactionEntry) super.withAmount(amount);
	}

	// TODO consider making this take the string, and using ofnullable to make it
	// more flexible on input, while still treating the data the same
	public TrackingTransactionEntry withTrackingAccount(Optional<String> trackingAccount) {
		var retval = (TrackingTransactionEntry) clone();
		retval.trackingAccount = trackingAccount;
		return retval;
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

}
