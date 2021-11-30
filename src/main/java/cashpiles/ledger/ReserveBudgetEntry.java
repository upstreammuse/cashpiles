package cashpiles.ledger;

import java.math.BigDecimal;

public class ReserveBudgetEntry extends BudgetEntry<ReserveBudgetEntry> {

	private String owner;
	private BigDecimal percentage;

	public ReserveBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public ReserveBudgetEntry(ReserveBudgetEntry other) {
		super(other);
		owner = other.owner;
		percentage = other.percentage;
	}

	public String owner() {
		return owner;
	}

	public BigDecimal percentage() {
		return percentage;
	}

	public ReserveBudgetEntry withOwner(String owner) {
		var retval = duplicate();
		retval.owner = owner;
		return retval;
	}

	public ReserveBudgetEntry withPercentage(BigDecimal percentage) {
		var retval = duplicate();
		retval.percentage = percentage;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

	@Override
	ReserveBudgetEntry asChild() {
		return this;
	}

	@Override
	ReserveBudgetEntry duplicate() {
		return new ReserveBudgetEntry(this);
	}

}
