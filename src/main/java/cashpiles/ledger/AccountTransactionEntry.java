package cashpiles.ledger;

public class AccountTransactionEntry extends TransactionEntry {

	public String account;

	public AccountTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	void balance(BalanceResult soFar) throws TransactionException {
		if (amount == null) {
			if (soFar.nullEntry == null) {
				soFar.nullEntry = this;
			} else {
				throw TransactionException.forMultipleEmptyEntries(this);
			}
		} else {
			soFar.accountTotal = soFar.accountTotal.add(amount);
		}
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
