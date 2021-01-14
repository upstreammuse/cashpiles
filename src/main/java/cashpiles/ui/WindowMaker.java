package cashpiles.ui;

import javax.swing.JFrame;

import cashpiles.budget.BudgetPeriod;

public class WindowMaker implements Windower {

	@Override
	public JFrame makeWindow(Windowable item) {
		return item.makeWindow(this);
	}

	@Override
	public JFrame makeWindow(BudgetPeriod period) {
		return new BudgetPeriodWindow(period);
	}

}
