package cashpiles.budget.ui;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;

import cashpiles.budget.BudgetException;
import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.ledger.IncomeBudgetEntry;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.ManualGoalBudgetEntry;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.ReserveBudgetEntry;
import cashpiles.ledger.RoutineBudgetEntry;
import cashpiles.ledger.Transaction;
import cashpiles.ledger.WithholdingBudgetEntry;
import cashpiles.model.Ledger;

@SuppressWarnings("serial")
public class BudgetPanel extends JPanel implements ItemProcessor {

	// TODO this member goes away once the panel can support being set multiple
	// times with different ledgers
	private boolean firstTime = true;
	private final BudgetPanelTableModel table = new BudgetPanelTableModel();

	public BudgetPanel() {
		initUI();
	}

	private void error(Exception ex) {
		JOptionPane.showMessageDialog(this, "Error processing budget.  " + ex.getLocalizedMessage(), "Budget Error",
				JOptionPane.ERROR_MESSAGE);
	}

	@Override
	public boolean process(Budget budget) {
		try {
			table.configureCurrentBudget(budget);
			return true;
		} catch (BudgetException ex) {
			error(ex);
			return false;
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
	public boolean process(Transaction transaction) {
		return true;
	}

	@Override
	public void process(WithholdingBudgetEntry entry) {
		try {
			table.configureCurrentBudget(entry);
		} catch (BudgetException ex) {
			error(ex);
		}
	}

	public void setLedger(Ledger ledger) throws LedgerException {
		if (firstTime) {
			ledger.process(this);
			firstTime = false;
		}
	}

	private void initUI() {
		var layout = new GroupLayout(this);
		setLayout(layout);
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
					BudgetPanel.this.table.makeWindow(row);
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
	}

}
