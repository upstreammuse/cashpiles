package cashpiles.budget.ui;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JTable;

import cashpiles.budget.BudgetException;
import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.ledger.IncomeBudgetEntry;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.ManualGoalBudgetEntry;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.ReserveBudgetEntry;
import cashpiles.ledger.RoutineBudgetEntry;
import cashpiles.ledger.WithholdingBudgetEntry;

@SuppressWarnings("serial")
public class BudgetWindow extends JFrame implements ItemProcessor {

	private final BudgetWindowTableModel table = new BudgetWindowTableModel();

	public BudgetWindow() {
		initUI();
	}

	private void error(Exception ex) {
		JOptionPane.showMessageDialog(this, "Error processing budget.  " + ex.getLocalizedMessage(), "Budget Error",
				JOptionPane.ERROR_MESSAGE);
	}

	@Override
	public void process(Budget budget) {
		try {
			table.configureCurrentBudget(budget);
		} catch (BudgetException ex) {
			error(ex);
		}
	}

	@Override
	public void process(CategoryTransactionEntry entry) {
		try {
			table.addTransaction(entry);
		} catch (BudgetException ex) {
			error(ex);
		}
	}

	@Override
	public void process(CloseBudgetEntry entry) {
		try {
			table.configureCurrentBudget(entry);
		} catch (BudgetException ex) {
			error(ex);
		}
	}

	@Override
	public void process(GoalBudgetEntry entry) {
		try {
			table.configureCurrentBudget(entry);
		} catch (BudgetException ex) {
			error(ex);
		}
	}

	@Override
	public void process(IncomeBudgetEntry entry) {
		try {
			table.configureCurrentBudget(entry);
		} catch (BudgetException ex) {
			error(ex);
		}
	}

	@Override
	public void process(ManualGoalBudgetEntry entry) {
		try {
			table.configureCurrentBudget(entry);
		} catch (BudgetException ex) {
			error(ex);
		}
	}

	@Override
	public void process(OwnerTransactionEntry entry) {
		try {
			table.addTransaction(entry);
		} catch (BudgetException ex) {
			error(ex);
		}
	}

	@Override
	public void process(ReserveBudgetEntry entry) {
		try {
			table.configureCurrentBudget(entry);
		} catch (BudgetException ex) {
			error(ex);
		}
	}

	@Override
	public void process(RoutineBudgetEntry entry) {
		try {
			table.configureCurrentBudget(entry);
		} catch (BudgetException ex) {
			error(ex);
		}
	}

	@Override
	public void process(WithholdingBudgetEntry entry) {
		try {
			table.configureCurrentBudget(entry);
		} catch (BudgetException ex) {
			error(ex);
		}
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
		table.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent event) {
				if (event.getClickCount() != 2) {
					return;
				}
				var row = table.rowAtPoint(event.getPoint());
				if (row != -1) {
					BudgetWindow.this.table.makeWindow(row);
				}
			}
		});
		JScrollPane scrollPane = new JScrollPane(table);
		table.setFillsViewportHeight(true);

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
