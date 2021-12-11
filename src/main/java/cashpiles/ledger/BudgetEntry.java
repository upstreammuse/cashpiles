package cashpiles.ledger;

public abstract class BudgetEntry extends LedgerItem {

	private String name;
	private Budget parent;

	public BudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String name() {
		return name;
	}

	public Budget parent() {
		return parent;
	}

	public BudgetEntry withName(String name) {
		var retval = clone();
		retval.name = name;
		return retval;
	}

	public BudgetEntry withParent(Budget parent) {
		var retval = clone();
		retval.parent = parent;
		return retval;
	}

	@Override
	public BudgetEntry clone() {
		var retval = (BudgetEntry) super.clone();
		// cloning removes the parent link because the parent doesn't know about the
		// clone
		retval.parent = null;
		return retval;
	}

}
