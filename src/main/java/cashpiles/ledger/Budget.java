package cashpiles.ledger;

import java.time.LocalDate;
import java.time.Period;
import java.util.ArrayList;
import java.util.List;

public class Budget extends LedgerItem {

	public LocalDate date;
	public List<BudgetEntry> entries = new ArrayList<>();
	public Period period;

	public Budget(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

}
