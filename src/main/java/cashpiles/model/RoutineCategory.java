package cashpiles.model;

import java.time.LocalDate;
import java.time.Period;
import java.time.temporal.ChronoUnit;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

class RoutineCategory extends Category {

	RoutineCategory(LocalDate startDate, DateRange startPeriod, String owner) {
		super(startDate, startPeriod, owner);
	}

	@Override
	public RoutineCategory clone() {
		return (RoutineCategory) super.clone();
	}

	@Override
	public String type() {
		return "Routine";
	}

	// FIXME it isn't clear when calling this whether or not the period is already
	// part of the list of periods or not, so we don't know whether to use size()-1
	// or start with size()-2. Just another issue with the general setup of the
	// allocation mechanism
	@Override
	BudgetPeriod allocate(BudgetPeriod period) {

		// FIXME these values need to come from the ledger!
		var aheadPeriod = Period.ofMonths(6);
		var behindGuess = new Amount();
		var behindPeriod = Period.ofYears(1);

		// FIXME in cases where the balance was negative and we corrected it, BUT the
		// daily amount is zero, we get back into a state where we are starting from
		// scratch, and thus we are probably iterating back and forth between a
		// corrected value and some other junk, and even/odd-ing whether we get a useful
		// looking number as a final allocation or not
		if (!period.allocation().equals(new Amount())) {
			return period;
		}

		Amount dailyAmount;
		if (size() < 2 || get(0).dates().startDate().plus(behindPeriod).compareTo(period.dates().startDate()) >= 0) {
			dailyAmount = behindGuess.distributeAndAdd(new DateRange(period.dates().startDate(), behindPeriod),
					new DateRange(period.dates().startDate(), Period.ofDays(1)));
		} else {
			var datesBehind = get(size() - 2).dates();
			var totalBehind = new Amount();
			for (var i = size() - 2; i >= 0
					&& get(i).dates().startDate().plus(behindPeriod).compareTo(period.dates().startDate()) > 0; i--) {
				datesBehind = new DateRange(get(i).dates().startDate(), datesBehind.endDate());
				totalBehind = totalBehind.add(get(i).activity().negate());
			}
			dailyAmount = totalBehind.distributeAndAdd(datesBehind,
					new DateRange(datesBehind.startDate(), Period.ofDays(1)));
		}
		var aheadAmount = dailyAmount.times(Math.toIntExact(
				ChronoUnit.DAYS.between(period.dates().startDate(), period.dates().startDate().plus(aheadPeriod))));

		// FIXME - this should correct if the balance falls below the lookahead amount?
		if (period.balance().isNegative()) {
			period = period.withAllocation(period.balance().negate().add(aheadAmount));
		}

		period = period.withAllocation(period.allocation().add(
				aheadAmount.distributeAndAdd(new DateRange(period.dates().startDate(), aheadPeriod), period.dates())));
		return period;
	}

}
