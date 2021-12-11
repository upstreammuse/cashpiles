package cashpiles.ledger;

import java.time.LocalDate;
import java.time.Period;
import java.util.ArrayList;
import java.util.List;

public class Budget extends DatedLedgerItem {

	private List<BudgetEntry> entries = new ArrayList<>();
	private Period period;

	public Budget(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public Period period() {
		return period;
	}

	public Budget withEntry(BudgetEntry entry) {
		var retval = clone();
		retval.entries.add(entry.withParent(retval));
		return retval;
	}

	public Budget withPeriod(Period period) {
		var retval = clone();
		retval.period = period;
		return retval;
	}

	@Override
	public Budget withDate(LocalDate date) {
		return (Budget) super.withDate(date);
	}

	@Override
	public void process(ItemProcessor processor) {
		if (processor.process(this)) {
			entries.stream().forEach(e -> {
				e.process(processor);
			});
		}
	}

	@Override
	public Budget clone() {
		var retval = (Budget) super.clone();
		entries = new ArrayList<>(entries);
		return retval;
	}

}
