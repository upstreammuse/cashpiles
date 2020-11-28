package cashpiles.ledger;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

public class Transaction extends LedgerItem {

	public enum Status {
		CLEARED, DISPUTED, PENDING
	};

	public LocalDate date;
	public List<TransactionEntry> entries = new ArrayList<>();
	public String payee;
	public Status status;

	public Transaction(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public void balance() throws MultipleEmptyEntriesException {
		var balancer = new TransactionEntry.BalanceResult();
		for (var entry : entries) {
			entry.balance(balancer);
		}
		if (balancer.nullEntry != null) {
			balancer.nullEntry.amount = balancer.missingAmount();
		}
	}

}
