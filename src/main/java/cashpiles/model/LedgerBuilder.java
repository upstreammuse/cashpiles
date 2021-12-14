package cashpiles.model;

import java.util.ArrayList;

import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.BlankLine;
import cashpiles.ledger.Budget;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.Transaction;
import cashpiles.ledger.UnbalancedTransaction;

public class LedgerBuilder implements ItemProcessor {

	private final Ledger ledger = new Ledger();
	private final ArrayList<LedgerException> pendingExceptions = new ArrayList<>();

	@Override
	public void process(AccountBalance balance) {
		try {
			ledger.add(balance);
		} catch (LedgerModelException ex) {
			pendingExceptions.add(ex);
		}
	}

	@Override
	public void process(AccountCommand command) {
		try {
			ledger.add(command);
		} catch (LedgerModelException ex) {
			pendingExceptions.add(ex);
		}
	}

	@Override
	public void process(BlankLine blank) {
		ledger.add(blank);
	}

	@Override
	public boolean process(Budget budget) {
		try {
			ledger.add(budget);
		} catch (LedgerException ex) {
			pendingExceptions.add(ex);
		}
		return false;
	}

	@Override
	public boolean process(Transaction transaction) {
		try {
			ledger.add(transaction);
		} catch (LedgerException ex) {
			pendingExceptions.add(ex);
		}
		return false;
	}

	@Override
	public void process(UnbalancedTransaction transaction) {
		try {
			ledger.add(transaction);
		} catch (LedgerModelException ex) {
			pendingExceptions.add(ex);
		}
	}

	public Ledger toLedger() throws LedgerException {
		if (!pendingExceptions.isEmpty()) {
			throw pendingExceptions.get(0);
		}
		return ledger;
	}

}
