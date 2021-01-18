package cashpiles.ui;

import javax.swing.JFrame;

import cashpiles.budget.BudgetPeriod;
import cashpiles.ui.budget.BudgetPeriodWindow;

public class WindowMaker implements Windower {

	@Override
	public JFrame makeWindow(Windowable item) {
		return item.makeWindow(this);
	}

	@Override
	public JFrame makeWindow(BudgetPeriod period) {
		var window = new BudgetPeriodWindow();
		window.setPeriod(period);
		return window;
	}

}
