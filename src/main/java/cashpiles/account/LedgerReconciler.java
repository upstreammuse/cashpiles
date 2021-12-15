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

class LedgerReconciler implements Cloneable, ItemProcessor {

	private List<LedgerItem> items = new ArrayList<>();
	private Transaction originalTransaction;
	private Transaction rebuiltTransaction;
	private AccountBalance statement;
	private List<AccountTransactionView> toClear = new ArrayList<>();
	private List<AccountTransactionView> toDefer = new ArrayList<>();

	private void addOld() throws TransactionException {
		if (originalTransaction != null) {
			items.add(originalTransaction);
		} else if (rebuiltTransaction != null) {
			rebuiltTransaction.balance();
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
	public boolean process(Transaction transaction) throws TransactionException {
		addOld();
		originalTransaction = transaction;
		rebuiltTransaction = new Transaction(transaction.fileName(), transaction.lineNumber(), transaction.comment())
				.withDate(transaction.date()).withPayee(transaction.payee()).withStatus(transaction.status());
		return true;
	}

	@Override
	public void process(UnbalancedTransaction transaction) throws TransactionException {
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
	public void processGeneric(LedgerItem item) throws TransactionException {
		addOld();
		items.add(item);
	}

	public Ledger toLedger() throws LedgerException {
		var retval = new Ledger();
		for (var item : items) {
			item.process(retval);
		}
		retval.process(statement);
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
