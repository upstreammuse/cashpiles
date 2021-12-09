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

	// TODO arguably some of this could go in the controller, since it's managing
	// the interactions between multiple components
	private void initController() {
		offBudgetAccounts.getSelectionModel().setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		offBudgetAccounts.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			for (int i = event.getFirstIndex(); i <= event.getLastIndex(); i++) {
				if (((ListSelectionModel) event.getSource()).isSelectedIndex(i)) {
					onBudgetAccounts.clearSelection();
					controller.selectOffBudget(i);
				}
			}
		});
		onBudgetAccounts.getSelectionModel().setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		onBudgetAccounts.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			for (int i = event.getFirstIndex(); i <= event.getLastIndex(); i++) {
				if (((ListSelectionModel) event.getSource()).isSelectedIndex(i)) {
					offBudgetAccounts.clearSelection();
					controller.selectOnBudget(i);
				}
			}
		});
		controller.forOnBudgetAccounts(onBudgetAccounts);
		controller.forOffBudgetAccounts(offBudgetAccounts);
		controller.forStatements(statements);
		controller.forTransactions(transactions);
		controller.onOnBudgetBalance(amount -> onBudgetBalance.setText("Balance: " + amount.toString() + "     "));
		controller.onOffBudgetBalance(amount -> offBudgetBalance.setText("Balance: " + amount.toString() + "     "));
	}

	private void initUI() {
		var layout = new GroupLayout(this);
		setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var onBudgetHeader = new JLabel("<html><b>On-Budget Accounts:</b></html>");
		onBudgetBalance.setMaximumSize(onBudgetBalance.getPreferredSize());
		var offBudgetHeader = new JLabel("<html><b>Off-Budget Accounts:</b></html>");
		offBudgetBalance.setMaximumSize(offBudgetBalance.getPreferredSize());
		var statementsHeader = new JLabel("<html><b>Statements</b></html>");
		var transactionsHeader = new JLabel("<html><b>Transactions</b></html>");

		transactions.setAutoCreateRowSorter(true);

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
