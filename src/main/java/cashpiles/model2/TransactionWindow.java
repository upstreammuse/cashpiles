package cashpiles.model2;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;

import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.GroupLayout.ParallelGroup;
import javax.swing.GroupLayout.SequentialGroup;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;

import com.github.lgooddatepicker.components.DatePicker;
import com.github.lgooddatepicker.components.DatePickerSettings;

import cashpiles.currency.Amount;

@SuppressWarnings("serial")
class TransactionWindow extends JFrame {

	private static class DataGroup {
		JComboBox<String> acctCat;
		JComboBox<String> trackingAcct;
		JTextField amount;
	}

	private ParallelGroup col1;
	private ParallelGroup col2;
	private ParallelGroup col3;
	private ParallelGroup col4;
	private List<DataGroup> data = new ArrayList<>();
	private DatePicker datePicker;
	private JLabel errorMsg;
	private Model model;
	private JTextField payee;
	private SequentialGroup rows;

	TransactionWindow(Model model) {
		this.model = model;
		initUI();
	}

	void addRow() {
		var pane = getContentPane();
		var layout = (GroupLayout) pane.getLayout();

		var info = new JLabel();
		var group = new DataGroup();
		group.acctCat = new JComboBox<String>();
		group.trackingAcct = new JComboBox<String>();
		group.amount = new JTextField();
		data.add(group);

		group.acctCat.addItem("");
		for (var accountName : model.accountNames(true)) {
			group.acctCat.addItem(accountName);
		}
		group.trackingAcct.setVisible(false);
		group.amount.setColumns(10);
		group.amount.setVisible(false);
		var row = layout.createParallelGroup(Alignment.BASELINE, false).addComponent(info).addComponent(group.acctCat)
				.addComponent(group.trackingAcct).addComponent(group.amount);

		group.acctCat.addActionListener(event -> {
			try {
				switch (model.getIdentifierType((String) group.acctCat.getSelectedItem())) {
				case ACCOUNT -> {
					info.setText("(account)");
					group.trackingAcct.setVisible(false);
				}
				case CATEGORY -> {
					info.setText("(category)");
					group.trackingAcct.setVisible(true);
				}
				case OWNER -> {
					info.setText("(owner)");
					group.trackingAcct.setVisible(true);
				}
				}
				group.amount.setVisible(true);
			} catch (ModelException ex) {
				info.setText("");
				group.trackingAcct.setVisible(false);
				group.amount.setVisible(false);
			}
		});

		rows.addGroup(row);
		col1.addComponent(info);
		col2.addComponent(group.acctCat);
		col3.addComponent(group.trackingAcct);
		col4.addComponent(group.amount);

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
		var settings = new DatePickerSettings();
		settings.setAllowEmptyDates(false);
		datePicker = new DatePicker(settings);
		datePicker.getComponentDateTextField().selectAll();

		var payeeLabel = new JLabel("Payee:");
		payee = new JTextField();
		payee.setColumns(30);

		errorMsg = new JLabel("   ");

		var addRowButton = new JButton("+");
		addRowButton.addActionListener(event -> addRow());
		var okButton = new JButton("OK");
		okButton.addActionListener(event -> render());
		var cancelButton = new JButton("Cancel");

		var rowH = layout.createSequentialGroup().addComponent(dateLabel).addComponent(datePicker)
				.addComponent(payeeLabel).addComponent(payee);
		var colsH = layout.createSequentialGroup().addGroup(col1).addGroup(col2).addGroup(col3).addGroup(col4);
		var dlgRowH = layout.createSequentialGroup().addComponent(okButton).addComponent(cancelButton);
		layout.setHorizontalGroup(layout.createParallelGroup(Alignment.TRAILING).addGroup(rowH)
				.addComponent(addRowButton).addGroup(colsH).addComponent(errorMsg).addGroup(dlgRowH));

		var rowV = layout.createParallelGroup(Alignment.BASELINE, false).addComponent(dateLabel)
				.addComponent(datePicker).addComponent(payeeLabel).addComponent(payee);
		var dlgRowV = layout.createParallelGroup(Alignment.BASELINE, false).addComponent(okButton)
				.addComponent(cancelButton);
		layout.setVerticalGroup(layout.createSequentialGroup().addGroup(rowV).addComponent(addRowButton).addGroup(rows)
				.addComponent(errorMsg).addGroup(dlgRowV));

		var label1 = new JLabel("Account / Category");
		var label2 = new JLabel("Off-budget Account");
		var label3 = new JLabel("Amount");

		rows.addGroup(layout.createParallelGroup().addComponent(label1).addComponent(label2).addComponent(label3));
		col2.addComponent(label1);
		col3.addComponent(label2);
		col4.addComponent(label3);

		SwingUtilities.getRootPane(okButton).setDefaultButton(okButton);
		pack();
		setMinimumSize(getPreferredSize());
	}

	void render() {
		System.out.println("render start");
		try {
			var xact = new Transaction(datePicker.getDate(), payee.getText());
			for (var group : data) {
				var acctCat = (String) group.acctCat.getSelectedItem();
				switch (model.getIdentifierType(acctCat)) {
				case ACCOUNT -> {
					xact = xact.withEntry(new AccountTransactionEntry(acctCat)
							.withAmount(new Amount(new BigDecimal(group.amount.getText()))));
				}
//				case CATEGORY -> 0;
//				case OWNER -> 0;
				default -> throw new IllegalArgumentException("Unexpected value: " + model.getIdentifierType(acctCat));
				}
			}
			model = model.withTransaction(xact);
		} catch (Exception ex) {
			errorMsg.setText(ex.getLocalizedMessage());
			pack();
			setMinimumSize(getPreferredSize());
		}
		System.out.println("render done");
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
