package cashpiles.ledger;

import java.time.LocalDate;

import cashpiles.currency.Amount;
import cashpiles.ledger.Transaction.Status;

public class UnbalancedTransaction extends LedgerItem {

	private String account;
	private Amount amount;
	private LocalDate date;
	private String payee;
	private Status status;

	public UnbalancedTransaction(String fileName, int lineNumber, String comment) {
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

	public String payee() {
		return payee;
	}

	public Status status() {
		return status;
	}

	public UnbalancedTransaction withAccount(String account) {
		var retval = (UnbalancedTransaction) clone();
		retval.account = account;
		return retval;
	}

	public UnbalancedTransaction withAmount(Amount amount) {
		var retval = (UnbalancedTransaction) clone();
		retval.amount = amount;
		return retval;
	}

	public UnbalancedTransaction withDate(LocalDate date) {
		var retval = (UnbalancedTransaction) clone();
		retval.date = date;
		return retval;
	}

	public UnbalancedTransaction withPayee(String payee) {
		var retval = (UnbalancedTransaction) clone();
		retval.payee = payee;
		return retval;
	}

	public UnbalancedTransaction withStatus(Status status) {
		var retval = (UnbalancedTransaction) clone();
		retval.status = status;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
