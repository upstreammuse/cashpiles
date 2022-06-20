package cashpiles.budget.redo2;

import java.util.Optional;

import javax.swing.GroupLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;

import cashpiles.model2.Ledger;
import cashpiles.model2.PeriodView;

@SuppressWarnings("serial")
public class BudgetPanel extends JPanel {

	private final JTable categoryTable = new JTable();
	private Ledger ledger = new Ledger();
	private final JTable periodTable = new JTable();
	private Optional<PeriodView> selectedPeriod = Optional.empty();

	public BudgetPanel() {
		initController();
		initUI();
	}

	private void initController() {
		periodTable.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			var selection = periodTable.getSelectedRows();
			if (selection.length >= 1) {
				selectedPeriod = Optional.of(ledger.getPeriods().get(periodTable.convertRowIndexToModel(selection[0])));
				refreshCategories();
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
		var catScroller = new JScrollPane(categoryTable);
		var periodLabel = new JLabel("Budget Periods");
		var periodScroller = new JScrollPane(periodTable);
		categoryTable.setAutoCreateRowSorter(true);
		periodTable.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

		layout.setHorizontalGroup(layout.createSequentialGroup()
				.addGroup(layout.createParallelGroup().addComponent(periodLabel).addComponent(periodScroller))
				.addGroup(layout.createParallelGroup().addComponent(catLabel).addComponent(catScroller)));
		layout.setVerticalGroup(layout.createParallelGroup()
				.addGroup(layout.createSequentialGroup().addComponent(periodLabel).addComponent(periodScroller))
				.addGroup(layout.createSequentialGroup().addComponent(catLabel).addComponent(catScroller)));
	}

	private void refresh() {
		periodTable.setModel(new PeriodTableModel(ledger.getPeriods()));
		selectedPeriod = Optional.empty();
		refreshCategories();
	}

	private void refreshCategories() {
		selectedPeriod.ifPresentOrElse(period -> categoryTable.setModel(new CategoryTableModel(period.categories())),
				() -> categoryTable.setModel(new CategoryTableModel()));
	}

	public void setLedger(Ledger ledger) {
		this.ledger = ledger;
		refresh();
	}

}
