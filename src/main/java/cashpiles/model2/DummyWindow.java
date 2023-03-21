package cashpiles.model2;

import java.awt.Color;
import java.math.BigDecimal;
import java.time.LocalDate;

import javax.swing.GroupLayout;
import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.SwingUtilities;
import javax.swing.UIDefaults;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import cashpiles.currency.Amount;

@SuppressWarnings("serial")
class DummyWindow extends JFrame {

	private Model model = new Model();
	private JTable accountOnTable = new JTable(new AccountsTableModel(model, true));

	void initUi() throws ModelException {
		setSize(320, 240);
		setTitle("CashPiles Dummy");
		setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		model = model.withOpenAccount("Checking", true);
		model = model.withOpenAccount("Savings", true);
		model = model.withOpenAccount("Retirement", false);
		var xact = new Transaction(LocalDate.now(), "Person 1");
		xact = xact.withEntry(new AccountTransactionEntry("Savings").withAmount(new Amount(new BigDecimal("-1000"))));
		xact = xact.withEntry(new AccountTransactionEntry("Checking"));
		xact = xact.balanced();
		model = model.withTransaction(xact);
		accountOnTable = new JTable(new AccountsTableModel(model, true));
		var scroller = new JScrollPane(accountOnTable);

		layout.setVerticalGroup(layout.createParallelGroup().addComponent(scroller));
		layout.setHorizontalGroup(layout.createSequentialGroup().addComponent(scroller));
		pack();
	}

	public static void main(String[] args) {
		SwingUtilities.invokeLater(() -> {
			try {
				UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
			} catch (ClassNotFoundException | InstantiationException | IllegalAccessException
					| UnsupportedLookAndFeelException ex) {
				// keep the default if the system L&F fails
			}
			System.setProperty("apple.laf.useScreenMenuBar", "true");
			UIDefaults defaults = UIManager.getLookAndFeelDefaults();
			if (defaults.get("Table.alternateRowColor") == null)
				defaults.put("Table.alternateRowColor", new Color(240, 240, 240));
			var dw = new DummyWindow();
			try {
				dw.initUi();
			} catch (ModelException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			dw.setVisible(true);
		});
	}

}
