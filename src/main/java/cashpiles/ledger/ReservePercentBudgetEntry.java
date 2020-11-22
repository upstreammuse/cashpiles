package cashpiles.ledger;

import java.math.BigDecimal;

public class ReservePercentBudgetEntry extends BudgetEntry {

	public String name;
	public String owner;
	public BigDecimal percentage;

	public ReservePercentBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

}
