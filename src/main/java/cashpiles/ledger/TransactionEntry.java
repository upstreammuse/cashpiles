package cashpiles.ledger;

import cashpiles.currency.Amount;

abstract public class TransactionEntry extends LedgerItem {

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
	private Transaction parent;

	public TransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public Amount amount() {
		return amount;
	}

	public Transaction parent() {
		return parent;
	}

	public TransactionEntry withAmount(Amount amount) {
		var retval = clone();
		retval.amount = amount;
		return retval;
	}

	public TransactionEntry withParent(Transaction parent) {
		var retval = clone();
		retval.parent = parent;
		return retval;
	}

	abstract void addToBalance(BalanceResult balancer) throws TransactionException;

	abstract TransactionEntry fromBalance(BalanceResult balancer);

	@Override
	public TransactionEntry clone() {
		return (TransactionEntry) super.clone();
	}

}
