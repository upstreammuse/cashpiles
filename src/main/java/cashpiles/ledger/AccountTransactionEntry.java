package cashpiles.ledger;

import java.time.LocalDate;

import cashpiles.currency.Amount;

public class AccountTransactionEntry extends TransactionEntry {

	private String account;

	public AccountTransactionEntry(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String account() {
		return account;
	}

	public AccountTransactionEntry withAccount(String account) {
		var retval = clone();
		retval.account = account;
		return retval;
	}

	@Override
	public AccountTransactionEntry withAmount(Amount amount) {
		return (AccountTransactionEntry) super.withAmount(amount);
	}

	@Override
	void addToBalance(Transaction.BalanceResult balancer) throws TransactionException {
		if (amount() == null) {
			if (balancer.emptyEntries < 1) {
				balancer.emptyEntries++;
			} else {
				throw TransactionException.forMultipleEmptyEntries(this);
			}
		} else {
			balancer.accountTotal = balancer.accountTotal.add(amount());
		}
	}

	@Override
	AccountTransactionEntry fromBalance(Transaction.BalanceResult balancer) {
		if (amount() == null) {
			var retval = withAmount(balancer.categoryTotal.add(balancer.accountTotal.negate()));
			balancer.accountTotal = balancer.categoryTotal;
			return retval;
		} else {
			return this;
		}
	}

	@Override
	public Amount accountAmount() {
		return amount();
	}

	@Override
	public LocalDate date() {
		return parent().date();
	}

	@Override
	public String payee() {
		return parent().payee();
	}

	@Override
	public void process(ItemProcessor processor) throws LedgerException {
		processor.process(this);
	}

	@Override
	public AccountTransactionEntry clone() {
		return (AccountTransactionEntry) super.clone();
	}

}
