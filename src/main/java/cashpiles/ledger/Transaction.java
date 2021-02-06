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
		var balancer = new TransactionEntry.BalanceResult();
		for (var entry : entries) {
			entry.balance(balancer);
		}
		if (balancer.nullEntry != null) {
			balancer.nullEntry.amount = balancer.missingAmount();

		}

		// TODO this is crap, but don't have a good way to apply the balance change to
		// the right place without rerunning the balance check
		balancer = new TransactionEntry.BalanceResult();
		for (var entry : entries) {
			entry.balance(balancer);
		}
		if (!balancer.accountTotal.equals(balancer.categoryTotal)) {
			throw TransactionException.forUnbalanced(this, balancer.accountTotal, balancer.categoryTotal);
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
