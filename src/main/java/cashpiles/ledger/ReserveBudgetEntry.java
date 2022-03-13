package cashpiles.ledger;

import java.math.BigDecimal;

public class ReserveBudgetEntry extends BudgetEntry {

	private String owner;
	private BigDecimal percentage;

	public ReserveBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String owner() {
		return owner;
	}

	public BigDecimal percentage() {
		return percentage;
	}

	public ReserveBudgetEntry withOwner(String owner) {
		var retval = clone();
		retval.owner = owner;
		return retval;
	}

	public ReserveBudgetEntry withPercentage(BigDecimal percentage) {
		var retval = clone();
		retval.percentage = percentage;
		return retval;
	}

	@Override
	public ReserveBudgetEntry withName(String name) {
		return (ReserveBudgetEntry) super.withName(name);
	}

	@Override
	public void process(ItemProcessor processor) throws LedgerException {
		processor.process(this);
	}

	@Override
	public ReserveBudgetEntry clone() {
		return (ReserveBudgetEntry) super.clone();
	}

}
