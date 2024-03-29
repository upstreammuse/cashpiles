package cashpiles.budget;

import java.util.Optional;

import javax.swing.GroupLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;

import cashpiles.model.Ledger;
import cashpiles.model.PeriodView;

@SuppressWarnings("serial")
public class BudgetPanel extends JPanel {

	private final JTable categoryTable = new JTable();
	private Ledger ledger = new Ledger();
	private final JTable ownerTable = new JTable();
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
				refreshOwners();
			}
		});
		refresh();
	}

	private void initUI() {
		var layout = new GroupLayout(this);
		setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var catLabel = new JLabel("<html><b>Categories</b></html>");
		var catScroller = new JScrollPane(categoryTable);
		var ownerLabel = new JLabel("<html><b>Category Owners</b></html>");
		var ownerScroller = new JScrollPane(ownerTable);
		var periodLabel = new JLabel("<html><b>Budget Periods</b></html>");
		var periodScroller = new JScrollPane(periodTable);
		categoryTable.setAutoCreateRowSorter(true);
		periodTable.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

		layout.setHorizontalGroup(layout.createSequentialGroup()
				.addGroup(layout.createParallelGroup().addComponent(periodLabel).addComponent(periodScroller))
				.addGroup(layout.createParallelGroup().addComponent(catLabel).addComponent(catScroller)
						.addComponent(ownerLabel).addComponent(ownerScroller)));
		layout.setVerticalGroup(layout.createParallelGroup()
				.addGroup(layout.createSequentialGroup().addComponent(periodLabel).addComponent(periodScroller))
				.addGroup(layout.createSequentialGroup().addComponent(catLabel).addComponent(catScroller)
						.addComponent(ownerLabel).addComponent(ownerScroller)));
	}

	private void refresh() {
		periodTable.setModel(new PeriodTableModel(ledger.getPeriods()));
		selectedPeriod = Optional.empty();
		refreshCategories();
		refreshOwners();
	}

	private void refreshCategories() {
		selectedPeriod.ifPresentOrElse(period -> categoryTable.setModel(new CategoryTableModel(period.categories())),
				() -> categoryTable.setModel(new CategoryTableModel()));
	}

	private void refreshOwners() {
		selectedPeriod.ifPresentOrElse(
				period -> ownerTable.setModel(new OwnerTableModel(period.categories(), period.owners())),
				() -> ownerTable.setModel(new OwnerTableModel()));
	}

	public void setLedger(Ledger ledger) {
		this.ledger = ledger;
		refresh();
	}

}
