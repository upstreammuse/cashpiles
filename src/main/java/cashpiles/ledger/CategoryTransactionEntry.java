package cashpiles.ledger;

public class CategoryTransactionEntry extends TrackingTransactionEntry {

	public String category;

	public CategoryTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
