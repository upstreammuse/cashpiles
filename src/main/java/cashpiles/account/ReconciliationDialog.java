package cashpiles.account;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;

import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JScrollPane;
import javax.swing.JTable;

import com.github.lgooddatepicker.components.DatePicker;
import com.github.lgooddatepicker.components.DatePickerSettings;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountTransactionView;
import cashpiles.model.StatementsView;

@SuppressWarnings("serial")
class ReconciliationDialog extends JDialog {

	private final JButton cancelButton = new JButton("Cancel");
	private final DatePicker datePicker = new DatePicker();
	private final ReconciliationDialogTableModel model;
	private boolean ok = false;
	private final JButton okButton = new JButton("Reconcile");
	private transient final Amount startBalance;
	private final JLabel selectedBalance = new JLabel(new Amount().toString());
	private transient Amount selectedTotal = new Amount();
	private final JTable table;

	ReconciliationDialog(JFrame parent, StatementsView view) {
		super(parent, true);
		model = new ReconciliationDialogTableModel(view);
		startBalance = view.get(view.size() - 1).startingBalance();
		selectedBalance.setText(startBalance.toString());
		table = new JTable(model);
		initController(view);
		initUI(parent);
	}

	LocalDate date() {
		return datePicker.getDate();
	}

	private void initController(StatementsView view) {
		cancelButton.addActionListener(event -> {
			dispose();
		});
		okButton.addActionListener(event -> {
			ok = true;
			dispose();
		});

		var dateSettings = new DatePickerSettings();
		datePicker.setSettings(dateSettings);
		dateSettings.setAllowEmptyDates(false);
		dateSettings.setDateRangeLimits(view.earliestReconcileDate(), null);
		datePicker.addDateChangeListener(event -> {
			var newDate = event.getNewDate();
			if (newDate != null) {
				model.setDate(newDate);
			}
		});
		model.setDate(datePicker.getDate());

		table.getSelectionModel().addListSelectionListener(event -> {
			if (event.getValueIsAdjusting()) {
				return;
			}
			selectedTotal = startBalance;
			for (int i = 0; i < table.getRowCount(); i++) {
				if (table.isRowSelected(i)) {
					selectedTotal = selectedTotal.add((Amount) table.getValueAt(i, 3));
				}
			}
			selectedBalance.setText(selectedTotal.toString());
		});
	}

	private void initUI(JFrame parent) {
		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var dateLabel = new JLabel("Statement Date");
		datePicker.setMaximumSize(datePicker.getPreferredSize());
		var scroller = new JScrollPane(table);
		var totalLabel = new JLabel("Statement Total");

		layout.setHorizontalGroup(layout.createParallelGroup()
				.addGroup(layout.createSequentialGroup()
						.addGroup(layout.createParallelGroup().addComponent(dateLabel).addComponent(datePicker))
						.addGap(0, 0, Integer.MAX_VALUE)
						.addGroup(layout.createParallelGroup(Alignment.TRAILING).addComponent(totalLabel)
								.addComponent(selectedBalance)))
				.addComponent(scroller).addGroup(Alignment.TRAILING,
						layout.createSequentialGroup().addComponent(cancelButton).addComponent(okButton)));
		layout.setVerticalGroup(layout.createSequentialGroup().addGroup(layout.createParallelGroup()
				.addGroup(layout.createSequentialGroup().addComponent(dateLabel).addComponent(datePicker))
				.addGroup(layout.createSequentialGroup().addComponent(totalLabel).addComponent(selectedBalance)))
				.addComponent(scroller)
				.addGroup(layout.createParallelGroup().addComponent(cancelButton).addComponent(okButton)));
		pack();
		setLocationRelativeTo(parent);
	}

	boolean ok() {
		return ok;
	}

	List<AccountTransactionView> selected() {
		var retval = new ArrayList<AccountTransactionView>();
		for (int i = 0; i < table.getRowCount(); i++) {
			if (table.isRowSelected(i)) {
				retval.add((AccountTransactionView) table.getValueAt(i, -1));
			}
		}
		return retval;
	}

	Amount selectedTotal() {
		return selectedTotal;
	}

	List<AccountTransactionView> unselected() {
		var retval = new ArrayList<AccountTransactionView>();
		for (int i = 0; i < table.getRowCount(); i++) {
			if (!table.isRowSelected(i)) {
				retval.add((AccountTransactionView) table.getValueAt(i, -1));
			}
		}
		return retval;
	}

}
