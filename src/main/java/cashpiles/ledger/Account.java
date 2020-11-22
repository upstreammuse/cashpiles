package cashpiles.ledger;

import java.time.LocalDate;

public class Account extends LedgerItem {

	public enum Status {
		CLOSED, OFF_BUDGET, ON_BUDGET
	}

	public LocalDate date;
	public String name;
	public Status status;

	public Account(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

}
