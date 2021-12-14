package cashpiles.account;

import javax.swing.GroupLayout;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;

import com.github.lgooddatepicker.components.DatePicker;

@SuppressWarnings("serial")
class TransactionDialog extends JDialog {

	private TransactionDialogTableModel tableModel = new TransactionDialogTableModel();
	private JTable entryTable = new JTable(tableModel);

	TransactionDialog(JFrame parent) {
		super(parent, true);
		initUI(parent);
	}

	private void initUI(JFrame parent) {
		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var dateLabel = new JLabel("Date");
		var datePicker = new DatePicker();
		var payeeField = new JTextField();
		var payeeLabel = new JLabel("Payee");
		var scrollPane = new JScrollPane(entryTable);
		var statusField = new JComboBox<>(new String[] { "Pending", "Cleared", "Disputed" });
		var statusLabel = new JLabel("Status");

		layout.setHorizontalGroup(layout.createParallelGroup()
				.addGroup(layout.createSequentialGroup()
						.addGroup(layout.createParallelGroup().addComponent(dateLabel).addComponent(datePicker))
						.addGroup(layout.createParallelGroup().addComponent(statusLabel).addComponent(statusField))
						.addGroup(layout.createParallelGroup().addComponent(payeeLabel).addComponent(payeeField)))
				.addComponent(scrollPane));
		layout.setVerticalGroup(layout.createSequentialGroup()
				.addGroup(layout.createParallelGroup()
						.addGroup(layout.createSequentialGroup().addComponent(dateLabel).addComponent(datePicker))
						.addGroup(layout.createSequentialGroup().addComponent(statusLabel).addComponent(statusField))
						.addGroup(layout.createSequentialGroup().addComponent(payeeLabel).addComponent(payeeField)))
				.addComponent(scrollPane));

		pack();
		setLocationRelativeTo(parent);
	}

}
