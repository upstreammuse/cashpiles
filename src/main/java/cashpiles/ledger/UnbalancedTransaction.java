package cashpiles.ledger;

import java.time.LocalDate;

import cashpiles.currency.Amount;
import cashpiles.ledger.Transaction.Status;

public class UnbalancedTransaction extends DatedLedgerItem implements AccountTransactionView {

	private String account;
	private Amount amount;
	int deferral = 0;
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

	public int deferral() {
		return deferral;
	}

	public String payee() {
		return payee;
	}

	public Status status() {
		return status;
	}

	public UnbalancedTransaction withAccount(String account) {
		var retval = clone();
		retval.account = account;
		return retval;
	}

	public UnbalancedTransaction withAmount(Amount amount) {
		var retval = clone();
		retval.amount = amount;
		return retval;
	}

	public UnbalancedTransaction withPayee(String payee) {
		var retval = clone();
		retval.payee = payee;
		return retval;
	}

	public UnbalancedTransaction withStatus(Status status) {
		var retval = clone();
		retval.status = status;
		return retval;
	}

	@Override
	public UnbalancedTransaction withDate(LocalDate date) {
		return (UnbalancedTransaction) super.withDate(date);
	}

	@Override
	public Amount accountAmount() {
		return amount();
	}

	@Override
	public void process(ItemProcessor processor) throws LedgerException {
		processor.process(this);
	}

	@Override
	public UnbalancedTransaction clone() {
		return (UnbalancedTransaction) super.clone();
	}

}
