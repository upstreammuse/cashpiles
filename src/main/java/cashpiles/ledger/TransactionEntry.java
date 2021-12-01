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

	// TODO this is package scope so that autobalancing can set it directly. Ideally
	// this could be private like the rest, but the autobalancing mechanism has to
	// be rewritten to work with original object references before that can work
	Amount amount;

	private Transaction parent;

	public TransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	// TODO should be able to eliminate these copy ctors and use clone instead
	public TransactionEntry(TransactionEntry other) {
		super(other);
		amount = other.amount;
		parent = other.parent;
	}

	public Amount amount() {
		return amount;
	}

	public Transaction parent() {
		return parent;
	}

	public TransactionEntry withAmount(Amount amount) {
		var retval = (TransactionEntry) clone();
		retval.amount = amount;
		return retval;
	}

	public TransactionEntry withParent(Transaction parent) {
		var retval = (TransactionEntry) clone();
		retval.parent = parent;
		return retval;
	}

	abstract void balance(BalanceResult soFar) throws TransactionException;

	abstract Amount missingAmount(BalanceResult balanceResult);

}
