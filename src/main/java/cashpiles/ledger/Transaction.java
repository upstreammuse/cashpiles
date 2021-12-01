package cashpiles.ledger;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

public class Transaction extends LedgerItem {

	public enum Status {
		CLEARED, DISPUTED, PENDING
	};

	private LocalDate date;
	private List<TransactionEntry> entries = new ArrayList<>();
	private String payee;
	private Status status;

	public Transaction(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public LocalDate date() {
		return date;
	}

	public String payee() {
		return payee;
	}

	public Status status() {
		return status;
	}

	public Transaction withDate(LocalDate date) {
		var retval = clone();
		retval.date = date;
		return retval;
	}

	public Transaction withEntry(TransactionEntry entry) {
		var retval = clone();
		retval.entries.add(entry.withParent(retval));
		return retval;
	}

	public Transaction withPayee(String payee) {
		var retval = clone();
		retval.payee = payee;
		return retval;
	}

	public Transaction withStatus(Status status) {
		var retval = clone();
		retval.status = status;
		return retval;
	}

	public void balance() throws TransactionException {
		var balancer = new TransactionEntry.BalanceResult();
		for (var entry : entries) {
			entry.addToBalance(balancer);
		}
		var entries2 = new ArrayList<TransactionEntry>();
		for (var entry : entries) {
			entries2.add(entry.fromBalance(balancer));
		}
		entries = entries2;
		balancer.confirmBalanced(this);
	}

	@Override
	public void process(ItemProcessor processor) {
		if (processor.process(this)) {
			entries.stream().forEach(e -> {
				e.process(processor);
			});
		}
	}

	@Override
	public Transaction clone() {
		var retval = (Transaction) super.clone();
		retval.entries = new ArrayList<>(entries);
		return retval;
	}

}
