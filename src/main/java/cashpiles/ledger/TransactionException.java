package cashpiles.ledger;

import cashpiles.currency.Amount;

@SuppressWarnings("serial")
public class TransactionException extends LedgerException {

	public static TransactionException forAccountWithOffBudget(AccountTransactionEntry entry) {
		return new TransactionException(entry,
				"Cannot use off-budget account " + entry.account() + " for account entries");
	}

	public static TransactionException forMultipleEmptyEntries(TransactionEntry entry) {
		return new TransactionException(entry,
				"Cannot automatically balance a transaction with more than one unspecified amount");
	}

	public static TransactionException forTrackingWithBudget(TrackingTransactionEntry entry) {
		var builder = new StringBuilder();
		entry.trackingAccount().ifPresentOrElse(trackingAccount -> builder.append(trackingAccount), () -> {
			throw new IllegalArgumentException("Tried to create a TransactionException with a missing account name");
		});
		return new TransactionException(entry,
				"Cannot use on-budget account " + builder + " for category/tracking entries");
	}

	public static TransactionException forUnbalanced(Transaction transaction, Amount accountTotal,
			Amount categoryTotal) {
		return new TransactionException(transaction,
				"Account total " + accountTotal + " and Category total " + categoryTotal + " do not match");
	}

	public static TransactionException forUnbalancedTransactionWithOnBudgetAccount(UnbalancedTransaction transaction) {
		return new TransactionException(transaction,
				"Cannot use single-line transaction with on-budget account '" + transaction.account + "'");
	}

	private TransactionException(LedgerItem item, String message) {
		super(item, message);
	}

}
