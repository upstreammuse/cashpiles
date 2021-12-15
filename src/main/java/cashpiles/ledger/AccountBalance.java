package cashpiles.ledger;

import java.time.LocalDate;

import cashpiles.currency.Amount;

public class AccountBalance extends DatedLedgerItem {

	private String account;
	private Amount amount;

	public AccountBalance(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String account() {
		return account;
	}

	public Amount amount() {
		return amount;
	}

	public AccountBalance withAccount(String account) {
		var retval = clone();
		retval.account = account;
		return retval;
	}

	public AccountBalance withAmount(Amount amount) {
		var retval = clone();
		retval.amount = amount;
		return retval;
	}

	@Override
	public AccountBalance withDate(LocalDate date) {
		return (AccountBalance) super.withDate(date);
	}

	@Override
	public void process(ItemProcessor processor) throws LedgerException {
		processor.process(this);
	}

	@Override
	public AccountBalance clone() {
		return (AccountBalance) super.clone();
	}

}
