package cashpiles.ledger;

import java.time.LocalDate;

import cashpiles.currency.Amount;
import cashpiles.ledger.Transaction.Status;

public class UnbalancedTransaction extends LedgerItem {

	public String account;
	public Amount amount;
	public LocalDate date;
	public String payee;
	public Status status;

	public UnbalancedTransaction(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
