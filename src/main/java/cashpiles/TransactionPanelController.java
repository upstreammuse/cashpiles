package cashpiles;

import java.math.BigDecimal;
import java.time.LocalDate;

import cashpiles.currency.Amount;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.Transaction;
import cashpiles.model.Ledger;

class TransactionPanelController {

	private final Ledger ledger;

	TransactionPanelController(TransactionPanelModel model, Ledger ledger) {
		this.ledger = ledger;
		ledger.addListener(action -> {
			model.clear();
			ledger.process(model);
		});
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
		} catch (LedgerException ex) {
			// TODO Auto-generated catch block
			ex.printStackTrace();
		}
		ledger.add(xact);
	}

}
