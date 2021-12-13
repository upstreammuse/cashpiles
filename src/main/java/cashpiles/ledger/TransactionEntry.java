package cashpiles.ledger;

import java.util.Optional;

import cashpiles.currency.Amount;

abstract public class TransactionEntry extends LedgerItem {

	// TODO move to transaction
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

	private Amount amount;
	private int deferral = 0;
	private Transaction parent;
	private Optional<Transaction.Status> status = Optional.empty();

	public TransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public Amount amount() {
		return amount;
	}

	public int deferral() {
		return deferral;
	}

	public Transaction parent() {
		return parent;
	}

	public Transaction.Status status() {
		return status.orElse(switch (parent.status()) {
		case CLEARED -> Transaction.Status.CLEARED;
		case DISPUTED -> Transaction.Status.CLEARED;
		case PENDING -> Transaction.Status.PENDING;
		});
	}

	public TransactionEntry withAmount(Amount amount) {
		var retval = clone();
		retval.amount = amount;
		return retval;
	}

	public TransactionEntry withDeferral(int deferral) {
		var retval = clone();
		retval.deferral = deferral;
		return retval;
	}

	public TransactionEntry withParent(Transaction parent) {
		var retval = clone();
		retval.parent = parent;
		return retval;
	}

	public TransactionEntry withStatus(Transaction.Status status) {
		var retval = clone();
		retval.status = Optional.of(status);
		return retval;
	}

	abstract void addToBalance(BalanceResult balancer) throws TransactionException;

	abstract TransactionEntry fromBalance(BalanceResult balancer);

	@Override
	public TransactionEntry clone() {
		var retval = (TransactionEntry) super.clone();
		// cloning decouples from the parent because the parent doesn't know about the
		// clone
		retval.parent = null;
		return retval;
	}

}
