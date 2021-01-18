package cashpiles.ui.budget;

import javax.swing.GroupLayout;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JTable;

import cashpiles.budget.BudgetPeriod;
import cashpiles.time.DateRange;

@SuppressWarnings("serial")
public class BudgetPeriodWindow extends JFrame {

	private final JLabel categoryActivity = new JLabel();
	private final JLabel categoryAllocation = new JLabel();
	private final JLabel categoryBalance = new JLabel();
	private final JTable categoryTable = new JTable();
	private final BudgetPeriodWindowController controller = new BudgetPeriodWindowController();
	private final JLabel ownerBalance = new JLabel();
	private final JTable ownerTable = new JTable();

	public BudgetPeriodWindow() {
		initController();
		initUI();
	}

	public void setPeriod(BudgetPeriod period) {
		controller.setPeriod(period);
		setTitle(period.dates());
	}

	private void initController() {
		controller.forCategoryTable(categoryTable);
		controller.forOwnerTable(ownerTable);
		controller.onCategoryActivity(amount -> categoryActivity.setText("Activity: " + amount.toString() + "     "));
		controller.onCategoryAllocation(
				amount -> categoryAllocation.setText("Allocation: " + amount.toString() + "     "));
		controller.onCategoryBalance(amount -> categoryBalance.setText("Balance: " + amount.toString() + "     "));
		controller.onOwnerBalance(amount -> ownerBalance.setText("Balance: " + amount.toString() + "     "));
	}

	private void initUI() {
		setSize(500, 400);
		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var categoryHeader = new JLabel("<html><b>Categories:</b></html>");
		categoryAllocation.setMaximumSize(categoryAllocation.getPreferredSize());
		categoryActivity.setMaximumSize(categoryActivity.getPreferredSize());
		categoryBalance.setMaximumSize(categoryBalance.getPreferredSize());
		var ownerHeader = new JLabel("<html><b>Owners:</b></html>");
		ownerBalance.setMaximumSize(ownerBalance.getPreferredSize());

		var scrollPane1 = new JScrollPane(categoryTable);
		categoryTable.setFillsViewportHeight(true);
		var separator = new JSeparator();
		var scrollPane2 = new JScrollPane(ownerTable);

		layout.setVerticalGroup(layout.createSequentialGroup()
				.addGroup(layout.createParallelGroup().addComponent(categoryHeader).addComponent(categoryAllocation)
						.addComponent(categoryActivity).addComponent(categoryBalance))
				.addComponent(scrollPane1).addComponent(separator)
				.addGroup(layout.createParallelGroup().addComponent(ownerHeader).addComponent(ownerBalance))
				.addComponent(scrollPane2));
		layout.setHorizontalGroup(layout.createParallelGroup()
				.addGroup(layout.createSequentialGroup().addComponent(categoryHeader).addComponent(categoryAllocation)
						.addComponent(categoryActivity).addComponent(categoryBalance))
				.addComponent(scrollPane1).addComponent(separator)
				.addGroup(layout.createSequentialGroup().addComponent(ownerHeader).addComponent(ownerBalance))
				.addComponent(scrollPane2));
		pack();
	}

	private void setTitle(DateRange dates) {
		setTitle("Budget Period " + dates.startDate() + " to " + dates.endDate());
	}

}
