package cashpiles.ledger;

public class CategoryTransactionEntry extends TrackingTransactionEntry {

	public String category;

	public CategoryTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	void balance(BalanceResult soFar) throws TransactionException {
		if (amount == null) {
			if (soFar.nullEntry == null) {
				soFar.nullEntry = this;
			} else {
				throw TransactionException.forMultipleEmptyEntries(this);
			}
		} else {
			soFar.categoryTotal = soFar.categoryTotal.add(amount);
		}
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
