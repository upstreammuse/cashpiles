package cashpiles.model;

import javax.swing.JOptionPane;

import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.Transaction;
import cashpiles.ledger.TransactionException;
import cashpiles.ledger.UnbalancedTransaction;

public class LedgerBuilder implements ItemProcessor {

	private final Ledger ledger;

	public LedgerBuilder(Ledger ledger) {
		this.ledger = ledger;
	}

	@Override
	public void process(AccountBalance balance) {
		try {
			ledger.add(balance);
		} catch (LedgerModelException ex) {
			error(ex);
		}
	}

	@Override
	public void process(AccountCommand command) {
		try {
			ledger.add(command);
		} catch (LedgerModelException ex) {
			error(ex);
		}
	}

	@Override
	public void process(AccountTransactionEntry entry) {
		try {
			ledger.add(entry);
		} catch (LedgerModelException ex) {
			error(ex);
		}
	}

	@Override
	public void process(CategoryTransactionEntry entry) {
		try {
			ledger.add(entry);
		} catch (LedgerModelException ex) {
			error(ex);
		}
	}

	@Override
	public void process(OwnerTransactionEntry entry) {
		try {
			ledger.add(entry);
		} catch (LedgerModelException ex) {
			error(ex);
		}
	}

	@Override
	public boolean process(Transaction transaction) {
		try {
			return ledger.add(transaction);
		} catch (TransactionException ex) {
			error(ex);
			return false;
		}
	}

	@Override
	public void process(UnbalancedTransaction transaction) {
		try {
			ledger.add(transaction);
		} catch (LedgerModelException ex) {
			error(ex);
		}
	}

	private void error(LedgerException ex) {
		JOptionPane.showMessageDialog(null, "Error processing ledger file.  " + ex.getLocalizedMessage(),
				"Ledger File Error", JOptionPane.ERROR_MESSAGE);
	}

}
