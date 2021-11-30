package cashpiles.ledger;

import java.time.LocalDate;
import java.time.Period;
import java.util.ArrayList;
import java.util.List;

public class Budget extends LedgerItem {

	private LocalDate date;
	private List<BudgetEntry<?>> entries = new ArrayList<>();
	private Period period;

	public Budget(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public Budget(Budget other) {
		super(other);
		date = other.date;
		entries = new ArrayList<BudgetEntry<?>>(other.entries);
		period = other.period;
	}

	public LocalDate date() {
		return date;
	}

	public Period period() {
		return period;
	}

	public Budget withDate(LocalDate date) {
		var retval = new Budget(this);
		retval.date = date;
		return retval;
	}

	public Budget withEntry(BudgetEntry<?> entry) {
		var retval = new Budget(this);
		retval.entries.add(entry.withParent(retval));
		return retval;
	}

	public Budget withPeriod(Period period) {
		var retval = new Budget(this);
		retval.period = period;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		if (processor.process(this)) {
			entries.stream().forEach(e -> {
				e.process(processor);
			});
		}
	}

}
