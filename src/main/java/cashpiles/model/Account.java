package cashpiles.model;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionView;
import cashpiles.util.Lists;

class Account extends ModelItem implements StatementsView {

	private final LocalDate startDate;
	private final AccountCommand.Status status;
	private List<Statement> statements = new ArrayList<>();

	Account(LocalDate startDate, AccountCommand.Status status) {
		this.startDate = startDate;
		this.status = status;
		statements.add(new Statement(new Amount()));
	}

	Amount balance() {
		return Lists.lastOf(statements).balance();
	}

	LocalDate startDate() {
		return startDate;
	}

	AccountCommand.Status status() {
		return status;
	}

	Account reconciled(AccountBalance balance) throws ModelException {
		var retval = clone();
		var reconciled = Lists.lastOf(retval.statements).withReconciliation(balance);
		var remaining = Lists.lastOf(retval.statements).withReconciliationRemainder(reconciled);
		retval.statements.remove(retval.statements.size() - 1);
		retval.statements.add(reconciled);
		retval.statements.add(remaining);
		return retval;
	}

	Account withTransaction(AccountTransactionView transaction) {
		var retval = clone();
		var it = retval.statements.listIterator(retval.statements.size() - 1);
		it.set(it.next().withTransaction(transaction));
		return retval;
	}

	@Override
	public Statement get(int index) {
		return statements.get(index);
	}

	@Override
	public int size() {
		return statements.size();
	}

	@Override
	public Account clone() {
		var retval = (Account) super.clone();
		retval.statements = new ArrayList<>(statements);
		return retval;
	}

	@Override
	public LocalDate earliestReconcileDate() {
		if (statements.size() >= 2) {
			// TODO consider a safer alternative here
			return statements.get(statements.size() - 2).closingDate().get();
		} else {
			return startDate;
		}
	}

}
