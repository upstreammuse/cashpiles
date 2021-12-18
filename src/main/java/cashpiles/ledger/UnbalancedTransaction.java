package cashpiles.ledger;

import java.time.LocalDate;

import cashpiles.currency.Amount;

public class UnbalancedTransaction extends DatedLedgerItem implements AccountTransactionView {

	private String account;
	private Amount amount;
	int deferral = 0;
	private String payee;
	private Transaction.Status status;

	public UnbalancedTransaction(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String account() {
		return account;
	}

	@Override
	public Transaction.Status accountStatus() {
		return status;
	}

	@Override
	public Amount amount() {
		return amount;
	}

	@Override
	public int deferral() {
		return deferral;
	}

	@Override
	public String payee() {
		return payee;
	}

	public Transaction.Status status() {
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

	public UnbalancedTransaction withStatus(Transaction.Status status) {
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
