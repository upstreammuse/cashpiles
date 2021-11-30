package cashpiles.ledger;

public abstract class BudgetEntry<T extends BudgetEntry<T>> extends LedgerItem {

	private String name;
	private Budget parent;

	public BudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public BudgetEntry(BudgetEntry<?> other) {
		super(other);
		name = other.name;
		parent = other.parent;
	}

	public String name() {
		return name;
	}

	public Budget parent() {
		return parent;
	}

	public T withName(String name) {
		var retval = duplicate();
		retval.name = name;
		return retval.asChild();
	}

	public T withParent(Budget parent) {
		var retval = duplicate();
		retval.parent = parent;
		return retval.asChild();
	}

	abstract T asChild();

	abstract BudgetEntry<T> duplicate();

}
