package cashpiles;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.math.BigDecimal;
import java.time.LocalDate;

import cashpiles.currency.Amount;
import cashpiles.ledger.Ledger;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.Transaction;
import cashpiles.ledger.TransactionException;

class TransactionPanelController implements ActionListener {

	private final Ledger ledger;
	private final TransactionPanelModel model;

	TransactionPanelController(TransactionPanelModel model, Ledger ledger) {
		this.ledger = ledger;
		this.model = model;
		ledger.addListener(this);
		ledger.process(model);
	}

	void newTransaction() {
		var xact = new Transaction("", 0, "").withDate(LocalDate.now()).withPayee("Test entry")
				.withStatus(Transaction.Status.DISPUTED);
		var entry = new OwnerTransactionEntry("", 0, "").withOwner("O1").withAmount(new Amount(new BigDecimal("10")));
		xact = xact.withEntry(entry);
		entry = new OwnerTransactionEntry("", 0, "").withOwner("O2");
		xact = xact.withEntry(entry);
		try {
			xact.balance();
		} catch (TransactionException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		ledger.add(xact);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		model.clear();
		((Ledger) e.getSource()).process(model);
	}

}
