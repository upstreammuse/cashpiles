package cashpiles.model;

import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;

@SuppressWarnings("serial")
public class LedgerModelException extends LedgerException {

	protected LedgerModelException(LedgerItem item, String message) {
		super(item, message);
	}

}
