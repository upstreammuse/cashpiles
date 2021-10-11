package cashpiles.ledger;

import java.time.LocalDate;

import cashpiles.currency.Amount;

public class AccountBalance extends LedgerItem {

	private String account;
	private Amount amount;
	private LocalDate date;

	public AccountBalance(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public AccountBalance(AccountBalance other) {
		super(other);
		account = other.account;
		amount = other.amount;
		date = other.date;
	}

	public String account() {
		return account;
	}

	public Amount amount() {
		return amount;
	}

	public LocalDate date() {
		return date;
	}

	public AccountBalance withAccount(String account) {
		AccountBalance retval = new AccountBalance(this);
		retval.account = account;
		return retval;
	}

	public AccountBalance withAmount(Amount amount) {
		AccountBalance retval = new AccountBalance(this);
		retval.amount = amount;
		return retval;
	}

	public AccountBalance withDate(LocalDate date) {
		AccountBalance retval = new AccountBalance(this);
		retval.date = date;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
