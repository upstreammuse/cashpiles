package cashpiles.budget.ui;

import java.util.List;
import java.time.LocalDate;
import java.time.Period;
import java.util.ArrayList;

import javax.swing.table.AbstractTableModel;

import cashpiles.budget.BudgetPeriod;
import cashpiles.budget.BudgetReconfigureException;
import cashpiles.ledger.Budget;
import cashpiles.ledger.LedgerItem;
import cashpiles.time.DateRange;
import cashpiles.util.Lists;

@SuppressWarnings("serial")
class BudgetWindowTableModel extends AbstractTableModel {

	private final List<BudgetPeriod> periods = new ArrayList<>();

	public void addItem(LedgerItem item) {
		// TODO implement
//		generateToDate(item.date);
	}

	public void configureCurrentBudget(Budget budget) throws BudgetReconfigureException {
		generateToDate(budget.date, budget.period);
		if (Lists.lastOf(periods).dates().startDate().equals(budget.date)) {
			Lists.lastOf(periods).setDates(new DateRange(budget.date, budget.period));
			fireTableDataChanged();
		} else {
			throw new BudgetReconfigureException(Lists.lastOf(periods).dates(), budget.date);
		}
	}

	void generateToDate(LocalDate date, Period period) {
		if (periods.isEmpty()) {
			periods.add(new BudgetPeriod(new DateRange(date, period)));
			fireTableDataChanged();
		}
		while (Lists.lastOf(periods).dates().endDate().compareTo(date) < 0) {
			generateNext();
		}
	}

	void generateNext() {
		periods.add(Lists.lastOf(periods).next());
		fireTableDataChanged();
	}

	@Override
	public int getColumnCount() {
		return 5;
	}

	@Override
	public String getColumnName(int index) {
		String[] headers = { "Start Date", "End Date", "Allocation", "Activity", "Balance" };
		return headers[index];
	}

	@Override
	public int getRowCount() {
		return periods.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		var period = periods.get(row);
		var dates = period.dates();
		if (col == 0) {
			return dates.startDate();
		} else if (col == 1) {
			return dates.endDate();
		} else if (col == 2) {
			return period.allocation();
		} else if (col == 3) {
			return period.activity();
		} else if (col == 4) {
			return period.balance();
		} else {
			throw new RuntimeException("Table index out of bounds");
		}
	}

}
