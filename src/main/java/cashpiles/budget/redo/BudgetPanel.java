package cashpiles.budget.redo;

import javax.swing.GroupLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;

import cashpiles.model.Ledger;

@SuppressWarnings("serial")
public class BudgetPanel extends JPanel {

	private final JTable categoryTable = new JTable();
	private Ledger ledger = new Ledger();

	public BudgetPanel() {
		initUI();
	}

	private void initUI() {
		var layout = new GroupLayout(this);
		setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var catLabel = new JLabel("Categories");
		var catScroller = new JScrollPane(categoryTable);

		layout.setHorizontalGroup(layout.createParallelGroup().addComponent(catLabel).addComponent(catScroller));
		layout.setVerticalGroup(layout.createSequentialGroup().addComponent(catLabel).addComponent(catScroller));
	}

	private void refresh() {
		categoryTable.setModel(new CategoryTableModel(ledger.getCategories()));
	}

	public void setLedger(Ledger ledger) {
		this.ledger = ledger;
		ledger.addListener(action -> refresh());
		refresh();
	}

}
