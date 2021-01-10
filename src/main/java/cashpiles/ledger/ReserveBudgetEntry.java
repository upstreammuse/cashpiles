package cashpiles.ledger;

import java.math.BigDecimal;

public class ReserveBudgetEntry extends BudgetEntry {

	public String name;
	public String owner;
	public BigDecimal percentage;

	public ReserveBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
