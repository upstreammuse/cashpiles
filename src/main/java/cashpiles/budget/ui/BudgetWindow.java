package cashpiles.budget.ui;

import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JTable;

import cashpiles.budget.BudgetReconfigureException;
import cashpiles.ledger.Budget;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerItem;

@SuppressWarnings("serial")
public class BudgetWindow extends JFrame implements ItemProcessor {

	private final BudgetWindowTableModel table = new BudgetWindowTableModel();

	public BudgetWindow() {
		initUI();
	}

	// TODO make this return boolean so to not handle sub items if the main item
	// doesn't process OK?
	public void process(Budget budget) {
		try {
			table.configureCurrentBudget(budget);
		} catch (BudgetReconfigureException ex) {
			JOptionPane.showMessageDialog(this, "Error configuring budget.  " + ex.getLocalizedMessage(),
					"Budget Configure Error", JOptionPane.ERROR_MESSAGE);
		}
	}

	public void process(LedgerItem item) {
		table.addItem(item);
	}

	private void initUI() {
		setSize(500, 400);
		setTitle("Budget Periods");

		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var table = new JTable(this.table);
		JScrollPane scrollPane = new JScrollPane(table);
		table.setFillsViewportHeight(true);
		add(scrollPane);

		var newbutton = new JButton("New Period");
		newbutton.addActionListener(event -> {
			System.out.println("TODO, prompt to be sure");
			this.table.generateNext();
		});

		var editbutton = new JButton("Edit Current Period");
		editbutton.addActionListener(event -> {
			System.out.println("Editing line " + (table.getRowCount() - 1));
		});

		layout.setVerticalGroup(layout.createSequentialGroup().addComponent(scrollPane)
				.addGroup(layout.createParallelGroup().addComponent(newbutton).addComponent(editbutton)));
		layout.setHorizontalGroup(layout.createParallelGroup().addComponent(scrollPane)
				.addGroup(layout.createSequentialGroup().addComponent(newbutton).addComponent(editbutton)));
		pack();
	}

}
