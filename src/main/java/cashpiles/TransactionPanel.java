package cashpiles;

import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;

import cashpiles.model.Ledger;

@SuppressWarnings("serial")
class TransactionPanel extends JPanel {

	private final TransactionPanelController controller;
	private final TransactionPanelModel model = new TransactionPanelModel();

	TransactionPanel(Ledger ledger) {
		controller = new TransactionPanelController(model, ledger);
		initUI();
	}

	private void initUI() {
		var layout = new GroupLayout(this);
		setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var table = new JTable(model);
		var scroller = new JScrollPane(table);

		var newXact = new JButton("New Transaction...");
		newXact.addActionListener(action -> {
			controller.newTransaction();
		});

		layout.setHorizontalGroup(layout.createParallelGroup().addComponent(scroller).addComponent(newXact));
		layout.setVerticalGroup(layout.createSequentialGroup().addComponent(scroller).addComponent(newXact));
	}

}
