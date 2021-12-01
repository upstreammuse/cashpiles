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
		// step 1, if there is an entry with an empty amount, calculate it and fill it
		// in
		var balancer = new TransactionEntry.BalanceResult();
		for (var entry : entries) {
			entry.balance(balancer);
		}
		balancer.emptyEntry.ifPresent(entry -> {
			entry.amount = balancer.missingAmount();
		});

		// step 2, make sure the accounts and categories match, which isn't guaranteed
		// if the transaction has everything populated but a number is wrong
		var balancer2 = new TransactionEntry.BalanceResult();
		for (var entry : entries) {
			entry.balance(balancer2);
		}
		if (!balancer2.accountTotal.equals(balancer2.categoryTotal)) {
			throw TransactionException.forUnbalanced(this, balancer2.accountTotal, balancer2.categoryTotal);
		}
	}

	@Override
	public Transaction clone() {
		var retval = (Transaction) super.clone();
		retval.entries = new ArrayList<>(entries);
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		if (processor.process(this)) {
			entries.stream().forEach(e -> {
				e.process(processor);
			});
		}
	}

}
