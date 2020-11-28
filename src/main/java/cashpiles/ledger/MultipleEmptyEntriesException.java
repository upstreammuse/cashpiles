package cashpiles.ledger;

@SuppressWarnings("serial")
public class MultipleEmptyEntriesException extends Exception {

	MultipleEmptyEntriesException() {
		super("Cannot automatically balance a transaction with more than one unspecified amount");
	}

}
