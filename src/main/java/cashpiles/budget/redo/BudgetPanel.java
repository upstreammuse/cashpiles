package cashpiles.budget.redo;

import java.util.Optional;

import javax.swing.GroupLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;

import cashpiles.model.Ledger;

@SuppressWarnings("serial")
public class BudgetPanel extends JPanel {

	private final JTable categoryTable = new JTable();
	private Ledger ledger = new Ledger();
	private final JTable periodTable = new JTable();
	private Optional<Integer> selectedCategory = Optional.empty();

	public BudgetPanel() {
		initController();
		initUI();
	}

	private void initController() {
		categoryTable.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			for (var i = event.getFirstIndex(); i <= event.getLastIndex(); i++) {
				if (categoryTable.isRowSelected(i)) {
					selectedCategory = Optional.of(i);
					refreshPeriods();
				}
			}
		});
		refresh();
	}

	private void initUI() {
		var layout = new GroupLayout(this);
		setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var catLabel = new JLabel("Categories");
		categoryTable.setAutoCreateRowSorter(true);
		categoryTable.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		var catScroller = new JScrollPane(categoryTable);
		var periodLabel = new JLabel("Budget Periods");
		var periodScroller = new JScrollPane(periodTable);

		layout.setHorizontalGroup(layout.createSequentialGroup()
				.addGroup(layout.createParallelGroup().addComponent(catLabel).addComponent(catScroller))
				.addGroup(layout.createParallelGroup().addComponent(periodLabel).addComponent(periodScroller)));
		layout.setVerticalGroup(layout.createParallelGroup()
				.addGroup(layout.createSequentialGroup().addComponent(catLabel).addComponent(catScroller))
				.addGroup(layout.createSequentialGroup().addComponent(periodLabel).addComponent(periodScroller)));
	}

	private void refresh() {
		categoryTable.setModel(new CategoryTableModel(ledger.getCategories()));
		selectedCategory = Optional.empty();
		refreshPeriods();
	}

	private void refreshPeriods() {
		selectedCategory.ifPresentOrElse(
				category -> periodTable.setModel(new PeriodTableModel(ledger.getCategories().get(category))),
				() -> periodTable.setModel(new PeriodTableModel()));
	}

	public void setLedger(Ledger ledger) {
		this.ledger = ledger;
		ledger.addListener(action -> refresh());
		refresh();
	}

}
