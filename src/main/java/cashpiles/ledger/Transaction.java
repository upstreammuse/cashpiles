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

	public void balance() throws TransactionException {
		// step 1, if there is an entry with an empty amount, calculate it and fill it
		// in
		final var balancer = new TransactionEntry.BalanceResult();
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
	public void process(ItemProcessor processor) {
		processor.process(this);
		entries.stream().forEach(e -> {
			e.parent = this;
			e.process(processor);
		});
	}

}
