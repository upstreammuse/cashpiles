package cashpiles.ledger;

public interface ItemProcessor {

	void process(Budget budget);
	void process(LedgerItem item);

}
