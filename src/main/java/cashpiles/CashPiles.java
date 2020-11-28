package cashpiles;

import java.awt.FileDialog;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.io.IOException;

import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;

import cashpiles.budget.ui.BudgetWindow;
import cashpiles.file.FileReader;
import cashpiles.file.IdentifierMismatchException;
import cashpiles.file.InvalidContentException;
import cashpiles.file.UnknownIdentifierException;
import cashpiles.ledger.MultipleEmptyEntriesException;

@SuppressWarnings("serial")
class CashPiles extends JFrame {

	CashPiles() {
		initUi();
	}

	private void initUi() {
		setSize(320, 240);
		setTitle("CashPiles");

		var menuBar = new JMenuBar();
		var fileMenu = new JMenu("File");

		var item = new JMenuItem("Open...");
		item.addActionListener(this::openFile);
		item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.META_MASK));
		fileMenu.add(item);

		menuBar.add(fileMenu);
		setJMenuBar(menuBar);

		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var openButton = new JButton("Open...");
		openButton.addActionListener(this::openFile);

		layout.setVerticalGroup(layout.createSequentialGroup().addComponent(openButton));
		layout.setHorizontalGroup(layout.createSequentialGroup().addComponent(openButton));
		pack();
	}

	private void openFile(ActionEvent event) {
		var dialog = new FileDialog(this);
		dialog.setFilenameFilter((dir, name) -> name.endsWith(".txt"));
		dialog.setVisible(true);
		var filename = dialog.getFile();
		if (filename != null) {
			try (var reader = new FileReader(dialog.getDirectory() + filename)) {
				var ledger = reader.readAll();
				var window = new BudgetWindow();
				ledger.process(window);
				window.setVisible(true);
			} catch (IOException | UnknownIdentifierException | IdentifierMismatchException
					| MultipleEmptyEntriesException | InvalidContentException ex) {
				JOptionPane.showMessageDialog(this, "Error reading file.  " + ex.getLocalizedMessage(),
						"File Read Error", JOptionPane.ERROR_MESSAGE);
			}
		}
	}

	public static void main(String[] args) {
		System.setProperty("apple.laf.useScreenMenuBar", "true");
		SwingUtilities.invokeLater(() -> {
			var cashPiles = new CashPiles();
			cashPiles.setVisible(true);
		});
	}

}
