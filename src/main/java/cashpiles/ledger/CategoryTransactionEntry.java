package cashpiles.ledger;

import cashpiles.currency.Amount;

public class CategoryTransactionEntry extends TrackingTransactionEntry {

	private String category;

	public CategoryTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String category() {
		return category;
	}

	public CategoryTransactionEntry withCategory(String category) {
		var retval = clone();
		retval.category = category;
		return retval;
	}

	@Override
	public CategoryTransactionEntry withAmount(Amount amount) {
		return (CategoryTransactionEntry) super.withAmount(amount);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

	@Override
	public CategoryTransactionEntry clone() {
		return (CategoryTransactionEntry) super.clone();
	}

}
