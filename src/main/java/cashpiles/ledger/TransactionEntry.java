package cashpiles.ledger;

import java.util.Optional;

import cashpiles.currency.Amount;

abstract public class TransactionEntry extends LedgerItem implements AccountTransactionView {

	private Amount amount;
	private int deferral = 0;
	private Transaction parent;
	private Optional<Transaction.Status> status = Optional.empty();

	public TransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	abstract void addToBalance(Transaction.BalanceResult balancer) throws TransactionException;

	@Override
	public Transaction.Status accountStatus() {
		return status.orElse(parent.status());
	}

	public Amount amount() {
		return amount;
	}

	@Override
	public TransactionEntry clone() {
		var retval = (TransactionEntry) super.clone();
		// cloning decouples from the parent because the parent doesn't know about the
		// clone
		retval.parent = null;
		return retval;
	}

	public int deferral() {
		return deferral;
	}

	abstract TransactionEntry fromBalance(Transaction.BalanceResult balancer);

	public Transaction parent() {
		return parent;
	}

	public Optional<Transaction.Status> status() {
		return status;
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

}
