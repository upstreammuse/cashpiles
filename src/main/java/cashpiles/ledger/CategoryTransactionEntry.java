package cashpiles.ledger;

public class CategoryTransactionEntry extends TrackingTransactionEntry {

	private String category;

	public CategoryTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public CategoryTransactionEntry(CategoryTransactionEntry other) {
		super(other);
		category = other.category;
	}

	public String category() {
		return category;
	}

	public CategoryTransactionEntry withCategory(String category) {
		var retval = new CategoryTransactionEntry(this);
		retval.category = category;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
