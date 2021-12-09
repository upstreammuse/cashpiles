package cashpiles.account;

import javax.swing.GroupLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;

import cashpiles.model.Ledger;

@SuppressWarnings("serial")
public class AccountsPanel extends JPanel {

	private final AccountsPanelController controller = new AccountsPanelController();
	private final JTable offBudgetAccounts = new JTable();
	private final JLabel offBudgetBalance = new JLabel();
	private final JTable onBudgetAccounts = new JTable();
	private final JLabel onBudgetBalance = new JLabel();
	private final JTable statements = new JTable();
	private final JTable transactions = new JTable();

	public AccountsPanel() {
		initController();
		initUI();
	}

	public void setLedger(Ledger ledger) {
		controller.setLedger(ledger);
	}

	private void initController() {
		controller.forAccounts(onBudgetAccounts, offBudgetAccounts);
		controller.forOffBudgetBalance(amount -> offBudgetBalance.setText("Balance: " + amount.toString() + "     "));
		controller.forOnBudgetBalance(amount -> onBudgetBalance.setText("Balance: " + amount.toString() + "     "));
		controller.forStatements(statements);
		controller.forTransactions(transactions);
	}

	private void initUI() {
		var layout = new GroupLayout(this);
		setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		offBudgetAccounts.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		onBudgetAccounts.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		statements.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		transactions.setAutoCreateRowSorter(true);

		var onBudgetHeader = new JLabel("<html><b>On-Budget Accounts:</b></html>");
		onBudgetBalance.setMaximumSize(onBudgetBalance.getPreferredSize());
		var offBudgetHeader = new JLabel("<html><b>Off-Budget Accounts:</b></html>");
		offBudgetBalance.setMaximumSize(offBudgetBalance.getPreferredSize());
		var statementsHeader = new JLabel("<html><b>Statements</b></html>");
		var transactionsHeader = new JLabel("<html><b>Transactions</b></html>");

		var scrollPane1 = new JScrollPane(onBudgetAccounts);
		var scrollPane2 = new JScrollPane(offBudgetAccounts);
		var scrollPane3 = new JScrollPane(statements);
		var scrollPane4 = new JScrollPane(transactions);

		layout.setVerticalGroup(layout.createParallelGroup().addGroup(layout.createSequentialGroup()
				.addGroup(layout.createParallelGroup().addComponent(onBudgetHeader).addComponent(onBudgetBalance))
				.addComponent(scrollPane1)
				.addGroup(layout.createParallelGroup().addComponent(offBudgetHeader).addComponent(offBudgetBalance))
				.addComponent(scrollPane2))
				.addGroup(layout.createSequentialGroup().addComponent(statementsHeader).addComponent(scrollPane3))
				.addGroup(layout.createSequentialGroup().addComponent(transactionsHeader).addComponent(scrollPane4)));
		layout.setHorizontalGroup(layout.createSequentialGroup().addGroup(layout.createParallelGroup()
				.addGroup(layout.createSequentialGroup().addComponent(onBudgetHeader).addComponent(onBudgetBalance))
				.addComponent(scrollPane1)
				.addGroup(layout.createSequentialGroup().addComponent(offBudgetHeader).addComponent(offBudgetBalance))
				.addComponent(scrollPane2))
				.addGroup(layout.createParallelGroup().addComponent(statementsHeader).addComponent(scrollPane3))
				.addGroup(layout.createParallelGroup().addComponent(transactionsHeader).addComponent(scrollPane4)));
	}

}
