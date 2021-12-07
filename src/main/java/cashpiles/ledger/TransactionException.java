package cashpiles.ledger;

import cashpiles.currency.Amount;

@SuppressWarnings("serial")
public class TransactionException extends LedgerException {

	static TransactionException forMultipleEmptyEntries(TransactionEntry entry) {
		return new TransactionException(entry,
				"Cannot automatically balance a transaction with more than one unspecified amount");
	}

	static TransactionException forUnbalanced(Transaction transaction, Amount accountTotal, Amount categoryTotal) {
		return new TransactionException(transaction,
				"Account total " + accountTotal + " and Category total " + categoryTotal + " do not match");
	}

	protected TransactionException(LedgerItem item, String message) {
		super(item, message);
	}

}
