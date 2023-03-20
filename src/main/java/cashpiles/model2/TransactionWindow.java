package cashpiles.model2;

import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.GroupLayout.ParallelGroup;
import javax.swing.GroupLayout.SequentialGroup;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JTextField;

import com.github.lgooddatepicker.components.DatePicker;

@SuppressWarnings("serial")
class TransactionWindow extends JFrame {

	private ParallelGroup col1;
	private ParallelGroup col2;
	private ParallelGroup col3;
	private ParallelGroup col4;
	private Model model;
	private SequentialGroup rows;

	TransactionWindow(Model model) {
		this.model = model;
		initUI();
	}

	void addRow() {
		var pane = getContentPane();
		var layout = (GroupLayout) pane.getLayout();

		var info = new JLabel();
		var acctCat = new JComboBox<String>();
		for (var accountName : model.accountNames(true)) {
			acctCat.addItem(accountName);
		}
		var trackingAcct = new JComboBox<String>();
		var amount = new JTextField();
		amount.setColumns(10);
		var row = layout.createParallelGroup(Alignment.BASELINE, false).addComponent(info).addComponent(acctCat)
				.addComponent(trackingAcct).addComponent(amount);

		acctCat.addActionListener(event -> {
			try {
				switch (model.getIdentifierType((String) acctCat.getSelectedItem())) {
				case ACCOUNT -> {
					info.setText("(account)");
					trackingAcct.setVisible(false);
				}
				case CATEGORY -> {
					info.setText("(category)");
					trackingAcct.setVisible(true);
				}
				case OWNER -> {
					info.setText("(owner)");
					trackingAcct.setVisible(true);
				}
				}
			} catch (ModelException ex) {
			}
		});

		rows.addGroup(row);
		col1.addComponent(info);
		col2.addComponent(acctCat);
		col3.addComponent(trackingAcct);
		col4.addComponent(amount);

		pack();
		setMinimumSize(getPreferredSize());
	}

	void initUI() {
		setSize(320, 240);
		setTitle("Edit Transaction");
		setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		col1 = layout.createParallelGroup();
		col2 = layout.createParallelGroup();
		col3 = layout.createParallelGroup();
		col4 = layout.createParallelGroup();
		rows = layout.createSequentialGroup();

		var dateLabel = new JLabel("Date:");
		var datePick = new DatePicker();
		var payeeLabel = new JLabel("Payee:");
		var payee = new JTextField();
		payee.setColumns(30);

		var rowH = layout.createSequentialGroup().addComponent(dateLabel).addComponent(datePick)
				.addComponent(payeeLabel).addComponent(payee);
		var colsH = layout.createSequentialGroup().addGroup(col1).addGroup(col2).addGroup(col3).addGroup(col4);
		layout.setHorizontalGroup(layout.createParallelGroup().addGroup(rowH).addGroup(colsH));

		var rowV = layout.createParallelGroup(Alignment.BASELINE, false).addComponent(dateLabel).addComponent(datePick)
				.addComponent(payeeLabel).addComponent(payee);
		rows.addGroup(rowV);
		layout.setVerticalGroup(rows);

		var label1 = new JLabel("Account / Category");
		var label2 = new JLabel("Off-budget Account");
		var label3 = new JLabel("Amount");

		rows.addGroup(layout.createParallelGroup().addComponent(label1).addComponent(label2).addComponent(label3));
		col2.addComponent(label1);
		col3.addComponent(label2);
		col4.addComponent(label3);

		pack();
		setMinimumSize(getPreferredSize());
	}

	public static void main(String[] args) throws ModelException {
		var model = new Model();
		model = model.withOpenAccount("Savings", true);
		model = model.withOpenAccount("Checking", true);
		model = model.withOpenAccount("Retirement", false);
		var asdf = new TransactionWindow(model);
		asdf.setVisible(true);
		asdf.addRow();
		asdf.addRow();
	}

}
