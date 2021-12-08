package cashpiles.model;

import javax.swing.JOptionPane;

import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.BlankLine;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.Transaction;
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
	public void process(BlankLine blank) {
		ledger.add(blank);
	}

	@Override
	public boolean process(Transaction transaction) {
		try {
			ledger.add(transaction);
		} catch (LedgerException ex) {
			error(ex);
		}
		// the ledger will process the entire transaction itself, so we don't need to
		// feed it the entries with the builder
		return false;
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
