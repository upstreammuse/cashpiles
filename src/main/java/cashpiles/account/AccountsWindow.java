package cashpiles.account;

import javax.swing.GroupLayout;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JTable;

import cashpiles.ledger.Account;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.ItemProcessor;

@SuppressWarnings("serial")
public class AccountsWindow extends JFrame implements ItemProcessor {

	private final AccountsWindowController controller = new AccountsWindowController();
	private final JTable offBudgetAccounts = new JTable();
	private final JLabel offBudgetBalance = new JLabel();
	private final JTable onBudgetAccounts = new JTable();
	private final JLabel onBudgetBalance = new JLabel();

	public AccountsWindow() {
		initController();
		initUI();
	}

	@Override
	public void process(Account account) {
		try {
			controller.process(account);
		} catch (AccountException ex) {
			error(ex);
		}
	}

	@Override
	public void process(AccountTransactionEntry entry) {
		try {
			controller.process(entry);
		} catch (AccountException ex) {
			error(ex);
		}
	}

	private void error(AccountException ex) {
		JOptionPane.showMessageDialog(this, "Error processing accounts.  " + ex.getLocalizedMessage(), "Account Error",
				JOptionPane.ERROR_MESSAGE);
	}

	private void initController() {
		controller.forOnBudgetAccounts(onBudgetAccounts);
		controller.forOffBudgetAccounts(offBudgetAccounts);
		controller.onOnBudgetBalance(amount -> onBudgetBalance.setText("Balance: " + amount.toString() + "     "));
		controller.onOffBudgetBalance(amount -> offBudgetBalance.setText("Balance: " + amount.toString() + "     "));
	}

	private void initUI() {
		setSize(500, 400);
		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var onBudgetHeader = new JLabel("<html><b>On-Budget Accounts:</b></html>");
		onBudgetBalance.setMaximumSize(onBudgetBalance.getPreferredSize());
		var offBudgetHeader = new JLabel("<html><b>Off-Budget Accounts:</b></html>");
		offBudgetBalance.setMaximumSize(offBudgetBalance.getPreferredSize());

		var scrollPane1 = new JScrollPane(onBudgetAccounts);
		onBudgetAccounts.setFillsViewportHeight(true);
		var separator = new JSeparator();
		var scrollPane2 = new JScrollPane(offBudgetAccounts);

		layout.setVerticalGroup(layout.createSequentialGroup()
				.addGroup(layout.createParallelGroup().addComponent(onBudgetHeader).addComponent(onBudgetBalance))
				.addComponent(scrollPane1).addComponent(separator)
				.addGroup(layout.createParallelGroup().addComponent(offBudgetHeader).addComponent(offBudgetBalance))
				.addComponent(scrollPane2));
		layout.setHorizontalGroup(layout.createParallelGroup()
				.addGroup(layout.createSequentialGroup().addComponent(onBudgetHeader).addComponent(onBudgetBalance))
				.addComponent(scrollPane1).addComponent(separator)
				.addGroup(layout.createSequentialGroup().addComponent(offBudgetHeader).addComponent(offBudgetBalance))
				.addComponent(scrollPane2));
		pack();
	}

}
