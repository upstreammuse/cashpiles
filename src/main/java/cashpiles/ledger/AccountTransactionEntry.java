package cashpiles.ledger;

public class AccountTransactionEntry extends TransactionEntry {

	public String account;

	public AccountTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	void balance(BalanceResult soFar) throws MultipleEmptyEntriesException {
		if (amount == null) {
			if (soFar.nullEntry == null) {
				soFar.nullEntry = this;
			} else {
				throw new MultipleEmptyEntriesException();
			}
		} else {
			soFar.accountTotal = soFar.accountTotal.add(amount);
		}
	}

}
