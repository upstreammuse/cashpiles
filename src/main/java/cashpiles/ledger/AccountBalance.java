package cashpiles.ledger;

import java.time.LocalDate;

import cashpiles.currency.Amount;

public class AccountBalance extends LedgerItem {

	public String account;
	public Amount amount;
	public LocalDate date;

	public AccountBalance(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}
	
	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
