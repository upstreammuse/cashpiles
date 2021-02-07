package cashpiles.ledger;

import java.util.Optional;

import cashpiles.currency.Amount;

abstract public class TransactionEntry extends LedgerItem {

	static class BalanceResult {
		Amount accountTotal = new Amount();
		Amount categoryTotal = new Amount();
		Optional<TransactionEntry> emptyEntry = Optional.empty();

		Amount missingAmount() {
			return emptyEntry.map(entry -> entry.missingAmount(this)).orElse(new Amount());
		}
	};

	public Amount amount;
	public Transaction parent;

	public TransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	abstract void balance(BalanceResult soFar) throws TransactionException;

	abstract Amount missingAmount(BalanceResult balanceResult);

}
