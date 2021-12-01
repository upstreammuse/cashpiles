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
		var retval = clone();
		retval.account = account;
		return retval;
	}

	public AccountBalance withAmount(Amount amount) {
		var retval = clone();
		retval.amount = amount;
		return retval;
	}

	public AccountBalance withDate(LocalDate date) {
		var retval = clone();
		retval.date = date;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

	@Override
	public AccountBalance clone() {
		return (AccountBalance) super.clone();
	}

}
