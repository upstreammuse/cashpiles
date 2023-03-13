package cashpiles.model2;

import java.time.LocalDate;
import java.time.Period;
import java.util.Map;
import java.util.NavigableMap;
import java.util.TreeMap;

class Model extends ModelBase {

	private Map<String, Account> accounts = new TreeMap<>();
	private NavigableMap<LocalDate, BudgetPeriod> budgetPeriods = new TreeMap<>();

	Model withHiddenAccount(String name) throws ModelException {
		if (!accounts.containsKey(name)) {
			throw ModelException.accountNotExist(name);
		}
		if (accounts.get(name).hidden()) {
			throw ModelException.accountNotOpen(name);
		}

		var model = clone();
		model.accounts.put(name, model.accounts.get(name).asHidden());
		return model;
	}

	Model withManualBudgetPeriod(LocalDate startDate, Period duration) throws ModelException {
		var model = clone();

		// remove all automatic periods
		// TODO get the transactions from these periods to restore them later
		model.budgetPeriods.entrySet().removeIf(entry -> !entry.getValue().manual());

		// make the new period
		var period = new BudgetPeriod(startDate, duration).asManual();

		// make sure none of the remaining (manual) periods overlap
		for (var entry : model.budgetPeriods.entrySet()) {
			if (entry.getValue().dates().overlaps(period.dates())) {
				throw ModelException.budgetPeriodOverlap(period.dates(), entry.getValue().dates());
			}
		}

		// put the new period in place
		model.budgetPeriods.put(startDate, period);

		// regenerate automatic periods by filling in the gaps between manual periods
		NavigableMap<LocalDate, BudgetPeriod> autoPeriods = new TreeMap<>();
		for (var periodIt = model.budgetPeriods.entrySet().iterator(); periodIt.hasNext(); /* inside */) {
			var manualPeriod = periodIt.next().getValue();

			// if the next manual period does not exist, we are done iterating
			var nextPeriodIt = periodIt;
			if (!nextPeriodIt.hasNext()) {
				break;
			}
			var nextPeriod = nextPeriodIt.next().getValue();

			// create automatic periods based on the current manual period as long as their
			// start dates are prior to the next manual period start date
			for (var autoPeriod = manualPeriod.next(); autoPeriod.dates().startDate()
					.compareTo(nextPeriod.dates().startDate()) < 0; autoPeriod = autoPeriod.next()) {

				// but the automatic period may extend into the next manual period, so cut it
				// short to just fit if it does
				if (autoPeriod.dates().endDate().compareTo(nextPeriod.dates().startDate()) >= 0) {
					autoPeriod = autoPeriod.withEndDate(nextPeriod.dates().startDate().minusDays(1));
				}
				autoPeriods.put(autoPeriod.dates().startDate(), autoPeriod);
			}
		}

		// merge the auto periods in with the manual periods
		model.budgetPeriods.putAll(autoPeriods);

		// TODO assign the transactions that were part of the prior set of automatic
		// periods

		return model;
	}

	Model withOpenAccount(String name, boolean onBudget) throws ModelException {
		var exists = accounts.containsKey(name);
		if (exists) {
			if (!accounts.get(name).hidden()) {
				throw ModelException.accountAlreadyOpen(name);
			}
			if (accounts.get(name).onBudget() != onBudget) {
				throw ModelException.accountTypeMismatch(name);
			}
		}

		var model = clone();
		if (exists) {
			model.accounts.put(name, model.accounts.get(name).asUnhidden());
		} else {
			model.accounts.put(name, new Account(name, onBudget));
		}
		return model;
	}

	// @Override
	protected Model clone() {
		var model = (Model) super.clone();
		model.accounts = new TreeMap<>(accounts);
		return model;
	}

}
