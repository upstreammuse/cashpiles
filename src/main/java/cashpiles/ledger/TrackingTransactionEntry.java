package cashpiles.ledger;

import java.util.Optional;

abstract public class TrackingTransactionEntry extends TransactionEntry {

	public Optional<String> trackingAccount = Optional.empty();

	public TrackingTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

}
