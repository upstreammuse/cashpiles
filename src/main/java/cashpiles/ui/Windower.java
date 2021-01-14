package cashpiles.ui;

import javax.swing.JFrame;

import cashpiles.budget.BudgetPeriod;

public interface Windower {

	JFrame makeWindow(Windowable windowable);

	JFrame makeWindow(BudgetPeriod period);

}
