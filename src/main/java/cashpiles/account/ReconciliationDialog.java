package cashpiles.account;

import javax.swing.GroupLayout;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JScrollPane;
import javax.swing.JTable;

import com.github.lgooddatepicker.components.DatePicker;
import com.github.lgooddatepicker.components.DatePickerSettings;

import cashpiles.currency.Amount;
import cashpiles.model.StatementsView;

@SuppressWarnings("serial")
class ReconciliationDialog extends JDialog {

	private final DatePicker datePicker = new DatePicker();
	private final ReconciliationDialogTableModel model;
	private final JLabel selectedBalance = new JLabel(new Amount().toString());
	private final JTable table;

	ReconciliationDialog(JFrame parent, StatementsView view) {
		super(parent, true);
		model = new ReconciliationDialogTableModel(view);
		table = new JTable(model);
		initController(view);
		initUI(parent);
	}

	private void initController(StatementsView view) {
		var dateSettings = new DatePickerSettings();
		datePicker.setSettings(dateSettings);
		dateSettings.setDateRangeLimits(view.earliestReconcileDate(), null);
		datePicker.addDateChangeListener(event -> {
			var newDate = event.getNewDate();
			if (newDate != null) {
				model.setDate(newDate);
			}
		});
		table.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			var total = new Amount();
			for (int i = 0; i < table.getRowCount(); i++) {
				if (table.isRowSelected(i)) {
					total = total.add((Amount) table.getValueAt(i, 3));
				}
			}
			selectedBalance.setText(total.toString());
		});
	}

	private void initUI(JFrame parent) {
		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		datePicker.setMaximumSize(datePicker.getPreferredSize());
		var scroller = new JScrollPane(table);

		layout.setHorizontalGroup(layout.createParallelGroup().addComponent(datePicker).addComponent(selectedBalance)
				.addComponent(scroller));
		layout.setVerticalGroup(layout.createSequentialGroup().addComponent(datePicker).addComponent(selectedBalance)
				.addComponent(scroller));

		pack();
		setLocationRelativeTo(parent);
	}

}
