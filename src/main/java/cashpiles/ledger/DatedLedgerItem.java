package cashpiles.ledger;

import java.time.LocalDate;

public abstract class DatedLedgerItem extends LedgerItem {

	private LocalDate date;

	public DatedLedgerItem(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public LocalDate date() {
		return date;
	}

	public DatedLedgerItem withDate(LocalDate date) {
		var retval = clone();
		retval.date = date;
		return retval;
	}

	@Override
	public DatedLedgerItem clone() {
		return (DatedLedgerItem) super.clone();
	}

}
