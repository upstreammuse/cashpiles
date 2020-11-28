package cashpiles.ledger;

import java.util.ArrayList;

@SuppressWarnings("serial")
public class Ledger extends ArrayList<LedgerItem> {

	public void process(ItemProcessor processor) {
		for (var item : this) {
			item.process(processor);
		}
	}

}
