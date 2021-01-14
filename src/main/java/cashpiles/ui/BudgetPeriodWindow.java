package cashpiles.ui;

import javax.swing.GroupLayout;
import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTable;

import cashpiles.budget.BudgetPeriod;
import cashpiles.time.DateRange;

@SuppressWarnings("serial")
class BudgetPeriodWindow extends JFrame {

	private BudgetPeriodModel model;

	BudgetPeriodWindow(BudgetPeriod period) {
		initUI(period);
	}

	private void initUI(BudgetPeriod period) {
		setSize(500, 400);
		setTitle(period.dates());

		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		model = new BudgetPeriodModel(period);
		var table = new JTable(model);
		JScrollPane scrollPane = new JScrollPane(table);
		table.setFillsViewportHeight(true);

		layout.setVerticalGroup(layout.createSequentialGroup().addComponent(scrollPane));
		layout.setHorizontalGroup(layout.createParallelGroup().addComponent(scrollPane));
		pack();
	}

	void setPeriod(BudgetPeriod period) {
		model.setPeriod(period);
		setTitle("Budget Period " + period.dates().startDate() + " to " + period.dates().endDate());
	}

	private void setTitle(DateRange dates) {
		setTitle("Budget Period " + dates.startDate() + " to " + dates.endDate());
	}

}
