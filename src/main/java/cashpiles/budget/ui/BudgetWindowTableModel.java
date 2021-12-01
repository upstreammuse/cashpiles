package cashpiles.budget.ui;

import java.time.LocalDate;
import java.time.Period;
import java.util.ArrayList;
import java.util.List;

import javax.swing.table.AbstractTableModel;

import cashpiles.budget.BudgetException;
import cashpiles.budget.BudgetPeriod;
import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.ledger.IncomeBudgetEntry;
import cashpiles.ledger.ManualGoalBudgetEntry;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.ReserveBudgetEntry;
import cashpiles.ledger.RoutineBudgetEntry;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.WithholdingBudgetEntry;
import cashpiles.time.DateRange;
import cashpiles.ui.budget.BudgetPeriodWindow;
import cashpiles.util.Lists;

@SuppressWarnings("serial")
class BudgetWindowTableModel extends AbstractTableModel {

	private final List<BudgetPeriod> periods = new ArrayList<>();

	void addTransaction(CategoryTransactionEntry entry) throws BudgetException {
		checkTransactionDate(entry);
		Lists.lastOf(periods).addTransaction(entry);
	}

	void addTransaction(OwnerTransactionEntry entry) throws BudgetException {
		checkTransactionDate(entry);
		Lists.lastOf(periods).addTransaction(entry);
	}

	void configureCurrentBudget(Budget budget) throws BudgetException {
		generateToDate(budget.date(), budget.period());
		if (Lists.lastOf(periods).dates().startDate().equals(budget.date())) {
			Lists.lastOf(periods).setDates(budget);
			fireTableDataChanged();
		} else {
			throw BudgetException.forReconfigure(budget, Lists.lastOf(periods).dates());
		}
	}

	void configureCurrentBudget(CloseBudgetEntry entry) throws BudgetException {
		Lists.lastOf(periods).configureCategory(entry);
	}

	public void configureCurrentBudget(GoalBudgetEntry entry) throws BudgetException {
		Lists.lastOf(periods).configureCategory(entry);
	}

	public void configureCurrentBudget(IncomeBudgetEntry entry) throws BudgetException {
		Lists.lastOf(periods).configureCategory(entry);
	}

	public void configureCurrentBudget(ManualGoalBudgetEntry entry) throws BudgetException {
		Lists.lastOf(periods).configureCategory(entry);
	}

	public void configureCurrentBudget(ReserveBudgetEntry entry) throws BudgetException {
		Lists.lastOf(periods).configureCategory(entry);
	}

	public void configureCurrentBudget(RoutineBudgetEntry entry) throws BudgetException {
		Lists.lastOf(periods).configureCategory(entry);
	}

	public void configureCurrentBudget(WithholdingBudgetEntry entry) throws BudgetException {
		Lists.lastOf(periods).configureCategory(entry);
	}

	void makeWindow(int row) {
		var window = new BudgetPeriodWindow();
		window.setPeriod(periods.get(row));
		window.setVisible(true);
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
			throw new IllegalArgumentException("Table index out of bounds");
		}
	}

	void generateNext() {
		periods.add(Lists.lastOf(periods).next());
		fireTableDataChanged();
	}

	private void generateToDate(LocalDate date) {
		if (periods.isEmpty()) {
			periods.add(new BudgetPeriod(new DateRange(date.withDayOfMonth(1), Period.ofMonths(1))));
			fireTableDataChanged();
		}
		while (Lists.lastOf(periods).dates().endDate().compareTo(date) < 0) {
			generateNext();
		}
	}

	private void generateToDate(LocalDate date, Period period) {
		if (periods.isEmpty()) {
			periods.add(new BudgetPeriod(new DateRange(date, period)));
			fireTableDataChanged();
		}
		while (Lists.lastOf(periods).dates().endDate().compareTo(date) < 0) {
			generateNext();
		}
	}

	private void checkTransactionDate(TrackingTransactionEntry entry) throws BudgetException {
		generateToDate(entry.parent().date());
		if (!Lists.lastOf(periods).dates().contains(entry.parent().date())) {
			throw BudgetException.forTransactionDate(entry, Lists.lastOf(periods).dates());
		}
	}

}
