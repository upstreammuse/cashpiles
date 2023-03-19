package cashpiles.model2;

import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.GroupLayout.ParallelGroup;
import javax.swing.GroupLayout.SequentialGroup;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JTextField;

@SuppressWarnings("serial")
class TransactionWindow extends JFrame {

	private ParallelGroup col1;
	private ParallelGroup col2;
	private ParallelGroup col3;
	private SequentialGroup rows;

	TransactionWindow() {
		initUI();
	}

	void addRow() {
		var pane = getContentPane();
		var layout = (GroupLayout) pane.getLayout();

		var asdf1 = new JComboBox<String>();
		var asdf2 = new JComboBox<String>();
		var asdf3 = new JTextField();
		var row = layout.createParallelGroup(Alignment.BASELINE, false).addComponent(asdf1).addComponent(asdf2)
				.addComponent(asdf3);

		rows.addGroup(row);
		col1.addComponent(asdf1);
		col2.addComponent(asdf2);
		col3.addComponent(asdf3);

		pack();
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
		rows = layout.createSequentialGroup();
		layout.setHorizontalGroup(layout.createSequentialGroup().addGroup(col1).addGroup(col2).addGroup(col3));
		layout.setVerticalGroup(layout.createSequentialGroup().addGroup(rows));

		var asdf1 = new JLabel("Account / Category");
		var asdf2 = new JLabel("Off-budget Account");
		var asdf3 = new JLabel("Amount");

		rows.addGroup(layout.createParallelGroup().addComponent(asdf1).addComponent(asdf2).addComponent(asdf3));
		col1.addComponent(asdf1);
		col2.addComponent(asdf2);
		col3.addComponent(asdf3);

		pack();
	}

	public static void main(String[] args) {
		var asdf = new TransactionWindow();
		asdf.setVisible(true);
		asdf.addRow();
		asdf.addRow();
	}

}
