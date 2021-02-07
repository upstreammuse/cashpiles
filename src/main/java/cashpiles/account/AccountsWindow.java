package cashpiles.account;

import javax.swing.GroupLayout;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;

import cashpiles.ledger.Account;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerException;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.UnbalancedTransaction;

@SuppressWarnings("serial")
public class AccountsWindow extends JFrame implements ItemProcessor {

	private final AccountsWindowController controller = new AccountsWindowController();
	private final JTable offBudgetAccounts = new JTable();
	private final JLabel offBudgetBalance = new JLabel();
	private final JTable onBudgetAccounts = new JTable();
	private final JLabel onBudgetBalance = new JLabel();
	private final JTable statements = new JTable();
	private final JTable transactions = new JTable();

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
	public void process(AccountBalance balance) {
		try {
			controller.process(balance);
		} catch (LedgerException ex) {
			error(ex);
		}
	}

	@Override
	public void process(AccountTransactionEntry entry) {
		try {
			controller.process(entry);
		} catch (LedgerException ex) {
			error(ex);
		}
	}

	@Override
	public void process(CategoryTransactionEntry entry) {
		try {
			controller.process(entry);
		} catch (LedgerException ex) {
			error(ex);
		}
	}

	@Override
	public void process(OwnerTransactionEntry entry) {
		try {
			controller.process(entry);
		} catch (LedgerException ex) {
			error(ex);
		}
	}

	@Override
	public void process(UnbalancedTransaction transaction) {
		try {
			controller.process(transaction);
		} catch (LedgerException ex) {
			error(ex);
		}
	}

	private void error(Exception ex) {
		JOptionPane.showMessageDialog(this, "Error processing accounts.  " + ex.getLocalizedMessage(), "Account Error",
				JOptionPane.ERROR_MESSAGE);
	}

	private void initController() {
		var offModel = offBudgetAccounts.getSelectionModel();
		offModel.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		offModel.addListSelectionListener(event -> {
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

		transactions.setAutoCreateRowSorter(true);

		var scrollPane1 = new JScrollPane(onBudgetAccounts);
		var scrollPane2 = new JScrollPane(offBudgetAccounts);
		var scrollPane3 = new JScrollPane(statements);
		var scrollPane4 = new JScrollPane(transactions);

		layout.setVerticalGroup(layout.createParallelGroup().addGroup(layout.createSequentialGroup()
				.addGroup(layout.createParallelGroup().addComponent(onBudgetHeader).addComponent(onBudgetBalance))
				.addComponent(scrollPane1)
				.addGroup(layout.createParallelGroup().addComponent(offBudgetHeader).addComponent(offBudgetBalance))
				.addComponent(scrollPane2)).addComponent(scrollPane3).addComponent(scrollPane4));

		layout.setHorizontalGroup(layout.createSequentialGroup().addGroup(layout.createParallelGroup()
				.addGroup(layout.createSequentialGroup().addComponent(onBudgetHeader).addComponent(onBudgetBalance))
				.addComponent(scrollPane1)
				.addGroup(layout.createSequentialGroup().addComponent(offBudgetHeader).addComponent(offBudgetBalance))
				.addComponent(scrollPane2)).addComponent(scrollPane3).addComponent(scrollPane4));

		pack();
	}

}
