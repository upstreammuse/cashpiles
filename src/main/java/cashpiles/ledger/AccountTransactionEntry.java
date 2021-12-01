package cashpiles.ledger;

import java.util.Optional;

import cashpiles.currency.Amount;

public class AccountTransactionEntry extends TransactionEntry {

	private String account;

	public AccountTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public AccountTransactionEntry(AccountTransactionEntry other) {
		super(other);
		account = other.account;
	}

	public String account() {
		return account;
	}

	public AccountTransactionEntry withAccount(String account) {
		var retval = new AccountTransactionEntry(this);
		retval.account = account;
		return retval;
	}

	@Override
	void balance(BalanceResult soFar) throws TransactionException {
		if (amount() == null) {
			if (soFar.emptyEntry.isEmpty()) {
				soFar.emptyEntry = Optional.of(this);
			} else {
				throw TransactionException.forMultipleEmptyEntries(this);
			}
		} else {
			soFar.accountTotal = soFar.accountTotal.add(amount());
		}
	}

	@Override
	Amount missingAmount(BalanceResult balanceResult) {
		return balanceResult.categoryTotal.add(balanceResult.accountTotal.negate());
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
