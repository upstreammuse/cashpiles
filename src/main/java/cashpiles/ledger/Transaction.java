package cashpiles.ledger;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;

public class Transaction extends DatedLedgerItem {

	static class BalanceResult {
		Amount accountTotal = new Amount();
		Amount categoryTotal = new Amount();
		int emptyEntries = 0;

		Amount confirmBalanced(Transaction transaction) throws TransactionException {
			if (!accountTotal.equals(categoryTotal)) {
				throw TransactionException.forUnbalanced(transaction, accountTotal, categoryTotal);
			}
			return accountTotal;
		}
	};

	public enum Status {
		CLEARED, DISPUTED, PENDING
	};

	private List<TransactionEntry> entries = new ArrayList<>();
	private String payee;
	private Status status;
	private Amount total;

	public Transaction(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String payee() {
		return payee;
	}

	public Status status() {
		return status;
	}

	public Amount total() {
		return total;
	}

	public Transaction withBalance() throws TransactionException {
		var retval = clone();
		var balancer = new BalanceResult();
		for (var entry : retval.entries) {
			entry.addToBalance(balancer);
		}
		var entries2 = new ArrayList<TransactionEntry>();
		for (var entry : retval.entries) {
			entries2.add(entry.fromBalance(balancer).withParent(retval));
		}
		retval.entries = entries2;
		retval.total = balancer.confirmBalanced(retval);

		// do not use status directly, because during the loop, the entries may call
		// parent().status() if they do not have explicit statuses set
		var newStatus = switch (retval.status) {
		case CLEARED -> Status.CLEARED;
		case DISPUTED -> Status.DISPUTED;
		case PENDING -> Status.CLEARED;
		};
		for (var entry : retval.entries) {
			newStatus = switch (newStatus) {
			case CLEARED -> switch (entry.accountStatus()) {
			case CLEARED -> Status.CLEARED;
			case DISPUTED -> Status.CLEARED;
			case PENDING -> Status.PENDING;
			};
			case DISPUTED -> newStatus;
			case PENDING -> newStatus;
			};
		}
		// update status once all entries are examined
		retval.status = newStatus;

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

	@Override
	public Transaction withDate(LocalDate date) {
		return (Transaction) super.withDate(date);
	}

	@Override
	public void process(ItemProcessor processor) throws LedgerException {
		if (processor.process(this)) {
			for (var entry : entries) {
				entry.process(processor);
			}
		}
	}

	@Override
	public Transaction clone() {
		var retval = (Transaction) super.clone();
		retval.entries = new ArrayList<>(entries);
		return retval;
	}

}
