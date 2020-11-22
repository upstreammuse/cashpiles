package cashpiles.ledger;

import java.time.LocalDate;

import cashpiles.currency.Amount;

public class GoalBudgetEntry extends BudgetEntry {

	public Amount amount;
	public String category;
	public LocalDate date;
	public String name;

	public GoalBudgetEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

}
