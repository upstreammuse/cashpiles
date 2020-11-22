package cashpiles.ledger;

import cashpiles.currency.Amount;

abstract public class TransactionEntry extends LedgerItem {

	static class BalanceResult {
		Amount accountTotal = new Amount();
		Amount categoryTotal = new Amount();
		TransactionEntry nullEntry;

		Amount missingAmount() {
			return accountTotal.add(categoryTotal).negate();
		}
	};

	public Amount amount;

	public TransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	abstract void balance(BalanceResult soFar) throws MultipleEmptyEntriesException;

}
