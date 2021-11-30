package cashpiles.ledger;

import java.util.Optional;

import cashpiles.currency.Amount;

abstract public class TrackingTransactionEntry extends TransactionEntry {

	public Optional<String> trackingAccount = Optional.empty();

	public TrackingTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public TrackingTransactionEntry(TrackingTransactionEntry other) {
		super(other);
		trackingAccount = other.trackingAccount;
	}

	@Override
	void balance(BalanceResult soFar) throws TransactionException {
		if (amount == null) {
			if (soFar.emptyEntry.isEmpty()) {
				soFar.emptyEntry = Optional.of(this);
			} else {
				throw TransactionException.forMultipleEmptyEntries(this);
			}
		} else {
			soFar.categoryTotal = soFar.categoryTotal.add(amount);
		}
	}

	@Override
	Amount missingAmount(BalanceResult balanceResult) {
		return balanceResult.accountTotal.add(balanceResult.categoryTotal.negate());
	}

}
