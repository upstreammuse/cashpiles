package cashpiles.ledger;

import cashpiles.currency.Amount;

abstract public class TransactionEntry extends LedgerItem {

	static class BalanceResult {
		Amount accountTotal = new Amount();
		Amount categoryTotal = new Amount();
		TransactionEntry nullEntry = null;

		// TODO fix this terrible way of doing things
		Amount missingAmount() {
			// can't answer if there is no missing entry
			if (nullEntry == null) {
				return new Amount();
			} else if (nullEntry instanceof AccountTransactionEntry) {
				return categoryTotal.add(accountTotal.negate());
			} else {
				return accountTotal.add(categoryTotal.negate());
			}
		}
	};

	public Amount amount;
	public Transaction parent;

	public TransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	abstract void balance(BalanceResult soFar) throws TransactionException;

}
