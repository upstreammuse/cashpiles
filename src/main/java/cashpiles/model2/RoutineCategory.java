package cashpiles.model2;

import java.time.Period;
import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.RoutineBudgetEntry;
import cashpiles.time.DateRange;

//this is an immutable data class
/**
 * We don't yet have a well defined method for routine to work, so here's what
 * we'll go with for now. First, the category will allocate (during the next
 * period generation) enough to equal the last 12 months of flow as applied to
 * the duration of the current period. Then, if the balance of the category is
 * below a 6 month average (based on the last 12 months), it will allocate an
 * additional amount to bring it up to the 6 month threshold. This is to
 * accomplish a few things:
 * 
 * 1. Make sure we allocate based on a rolling average of past performance
 * 
 * 2. Make sure we always have enough to cover the next 6 months of average
 * spending
 * 
 * There are issues with this, because the point of covering the next 6 months
 * is to cover "cost of living" in the absence of additional income during that
 * time. However, the program doesn't fill up routine categories based on income
 * events, so there's no way to tie the routine allocation to getting money. It
 * may be that we need to connect routine allocation to an income event, but
 * that gets awkward because we only want to do the allocation for the first
 * income event in a budget period, assuming that income is sufficient to fund
 * the routine categories.
 * 
 * Another possible approach is to only fund routine categories for "now", i.e.
 * the current budget period, based on average past performance, but don't try
 * to also allocate the 6 month lookahead. Instead, the program could keep a
 * global sum of what the total 6 month (or some other duration) lookahead is,
 * and show that to the user. That would allow a new user to start with a small
 * lookahead as low as 0 days, and encourage the user to increase the lookahead
 * amount until it's at least 180 days or so.
 * 
 * However, all routine categories would then fall under a single lookahead
 * number, making it difficult to understand which categories are contributing
 * the most to the lookahead amount. It is almost certain that users will have
 * multiple "tiers" of routine expenses, and would be willing and able to cut
 * off some of those expenses during an actual loss of income.
 */
class RoutineCategory extends Category {

	private List<CategoryTransactionEntry> oldTransactions = new ArrayList<>();

	RoutineCategory(RoutineBudgetEntry entry) {
		super(entry.owner());
	}

	@Override
	public RoutineCategory clone() {
		var retval = (RoutineCategory) super.clone();
		retval.oldTransactions = new ArrayList<>(oldTransactions);
		return retval;
	}

	@Override
	RoutineCategory next(DateRange dates) {
		var retval = (RoutineCategory) super.next(dates);

		// keep any old transactions that are within a year of the new period start date
		var oldTransactions = new ArrayList<CategoryTransactionEntry>();
		for (var xact : transactions) {
			if (xact.date().plus(Period.ofYears(1)).compareTo(dates.startDate()) >= 0) {
				oldTransactions.add(xact);
			}
		}
		for (var xact : this.oldTransactions) {
			if (xact.date().plus(Period.ofYears(1)).compareTo(dates.startDate()) >= 0) {
				oldTransactions.add(xact);
			}
		}
		retval.oldTransactions = oldTransactions;

		// get the total of the old transactions
		var yearTotal = new Amount();
		for (var xact : retval.oldTransactions) {
			yearTotal = yearTotal.add(xact.amount());
		}

		// distribute the yearly total over a single day to determine the daily average
		var dayAverage = yearTotal.distributeAndAdd(
				new DateRange(dates.startDate().minus(Period.ofYears(1)), dates.startDate()),
				new DateRange(dates.startDate(), dates.startDate()));

		// allocate based on the new period duration
		// FIXME make sure other category allocations don't add to existing allocation,
		// since it doesn't get reset during a clone of a category
		retval.allocation = dayAverage.times(dates.numberOfDays());

		// If the balance is below the 6 month average, pull it up to be at least the 6
		// month average. This lets the balance get larger than the 6 month average, for
		// routine expenses that happen less frequently (like annual bills), but ensures
		// that at least the next 6 months are covered.
		var sixMonthTotal = dayAverage.times(180);
		if (retval.allocation.add(sixMonthTotal.negate()).isNegative()) {
			retval.allocation = sixMonthTotal;
		}

		return retval;
	}

}
