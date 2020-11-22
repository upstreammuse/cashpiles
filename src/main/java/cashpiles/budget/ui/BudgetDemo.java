package cashpiles.budget.ui;

import java.time.LocalDate;
import java.time.Period;

import javax.swing.SwingUtilities;

import cashpiles.budget.BudgetPeriod;
import cashpiles.time.DateRange;

public class BudgetDemo {

	public static void main(String[] args) {
		SwingUtilities.invokeLater(() -> {
			var date = LocalDate.now().withDayOfMonth(1);
			var duration = Period.ofMonths(1);
			var bp = new BudgetPeriod(new DateRange(date, duration));
			var window = new BudgetWindow(bp);
			window.setVisible(true);
		});
	}

}
