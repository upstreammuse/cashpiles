package cashpiles.model2;

import cashpiles.ledger.RoutineBudgetEntry;

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

	RoutineCategory(RoutineBudgetEntry entry) {
		super(entry.owner());
	}

}
