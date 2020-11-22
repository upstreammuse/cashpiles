package cashpiles.ledger;

public class OwnerTransactionEntry extends TrackingTransactionEntry {

	public String owner;

	public OwnerTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	void balance(BalanceResult soFar) throws MultipleEmptyEntriesException {
		if (amount == null) {
			if (soFar.nullEntry == null) {
				soFar.nullEntry = this;
			} else {
				throw new MultipleEmptyEntriesException();
			}
		} else {
			soFar.categoryTotal = soFar.categoryTotal.add(amount);
		}
	}

}
