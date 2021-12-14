package cashpiles.account;

import java.util.ArrayList;
import java.util.List;

import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.AccountTransactionView;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.Transaction;
import cashpiles.ledger.TransactionEntry;
import cashpiles.ledger.TransactionException;
import cashpiles.ledger.UnbalancedTransaction;
import cashpiles.model.Ledger;
import cashpiles.model.LedgerBuilder;

class LedgerReconciler implements Cloneable, ItemProcessor {

	private List<LedgerItem> items = new ArrayList<>();
	private Transaction originalTransaction;
	private List<TransactionException> pendingExceptions = new ArrayList<>();
	private Transaction rebuiltTransaction;
	private AccountBalance statement;
	private List<AccountTransactionView> toClear = new ArrayList<>();
	private List<AccountTransactionView> toDefer = new ArrayList<>();

	private void addOld() {
		if (originalTransaction != null) {
			items.add(originalTransaction);
		} else if (rebuiltTransaction != null) {
			try {
				rebuiltTransaction.balance();
			} catch (TransactionException ex) {
				pendingExceptions.add(ex);
			}
			items.add(rebuiltTransaction);
		}
		originalTransaction = null;
		rebuiltTransaction = null;
	}

	@Override
	public LedgerReconciler clone() {
		try {
			var retval = (LedgerReconciler) super.clone();
			retval.items = new ArrayList<>(items);
			retval.pendingExceptions = new ArrayList<>(pendingExceptions);
			retval.toClear = new ArrayList<>(toClear);
			retval.toDefer = new ArrayList<>(toDefer);
			return retval;
		} catch (CloneNotSupportedException ex) {
			// TODO have a common system for what to do here for cloneable top level parents
			throw new RuntimeException(ex);
		}
	}

	@Override
	public void process(AccountTransactionEntry entry) {
		processEntry(entry);
	}

	@Override
	public void process(CategoryTransactionEntry entry) {
		processEntry(entry);
	}

	@Override
	public void process(OwnerTransactionEntry entry) {
		processEntry(entry);
	}

	private void processEntry(TransactionEntry entry) {
		if (toClear.contains(entry)) {
			rebuiltTransaction = rebuiltTransaction.withEntry(entry.withStatus(Transaction.Status.CLEARED));
			originalTransaction = null;
		} else if (toDefer.contains(entry)) {
			rebuiltTransaction = rebuiltTransaction.withEntry(entry.withDeferral(entry.deferral() + 1));
			originalTransaction = null;
		} else {
			rebuiltTransaction = rebuiltTransaction.withEntry(entry);
		}
	}

	@Override
	public boolean process(Transaction transaction) {
		addOld();
		originalTransaction = transaction;
		rebuiltTransaction = new Transaction(transaction.fileName(), transaction.lineNumber(), transaction.comment())
				.withDate(transaction.date()).withPayee(transaction.payee()).withStatus(transaction.status());
		return true;
	}

	@Override
	public void process(UnbalancedTransaction transaction) {
		addOld();
		if (toClear.contains(transaction)) {
			items.add(transaction.withStatus(Transaction.Status.CLEARED));
		} else if (toDefer.contains(transaction)) {
			// FIXME unbalanced transactions can't have their deferral set
		} else {
			items.add(transaction);
		}
	}

	@Override
	public void processGeneric(LedgerItem item) {
		addOld();
		items.add(item);
	}

	// TODO ledger builder could work in a similar way, although each time we do
	// this we are less efficient because we keep making all these copies
	// - as a benefit to doing it this way, the toLedger call could throw,
	// preventing a bum ledger from making its way through the system
	public Ledger toLedger() throws LedgerException {
		if (!pendingExceptions.isEmpty()) {
			throw pendingExceptions.get(0);
		}
		var retval = new Ledger();
		var builder = new LedgerBuilder(retval);
		for (var item : items) {
			item.process(builder);
		}
		retval.add(statement);
		return retval;
	}

	public LedgerReconciler withClearedTransaction(AccountTransactionView transaction) {
		var retval = clone();
		retval.toClear.add(transaction);
		return retval;
	}

	public LedgerReconciler withDeferredTransaction(AccountTransactionView transaction) {
		var retval = clone();
		retval.toDefer.add(transaction);
		return retval;
	}

	public LedgerReconciler withStatement(AccountBalance balance) {
		var retval = clone();
		retval.statement = balance;
		return retval;
	}

}
