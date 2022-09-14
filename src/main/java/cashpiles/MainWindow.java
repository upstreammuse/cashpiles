package cashpiles;

import java.awt.FileDialog;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;

import javax.swing.GroupLayout;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JTabbedPane;
import javax.swing.KeyStroke;

import cashpiles.account.AccountsPanel;
import cashpiles.budget.BudgetPanel;
import cashpiles.file.LedgerReader;
import cashpiles.file.LedgerWriter;
import cashpiles.ledger.LedgerException;
import cashpiles.model.Ledger;

// TODO look into the Preferences class to store user settings in a platform-matching way
@SuppressWarnings("serial")
public class MainWindow extends JFrame {

	private final AccountsPanel accountsPanel = new AccountsPanel(this);
	private final BudgetPanel budgetPanel = new BudgetPanel();
	private transient Ledger ledger = new Ledger();

	public void initUi() {
		setSize(320, 240);
		setTitle("CashPiles");
		setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

		var menuBar = new JMenuBar();
		setJMenuBar(menuBar);
		var shortcutKeyCode = Toolkit.getDefaultToolkit().getMenuShortcutKeyMaskEx();

		var fileMenu = new JMenu("File");
		fileMenu.setMnemonic(KeyEvent.VK_F);
		menuBar.add(fileMenu);

		// TODO implement this eventually, but for now just work with existing files
//		var menuItem = new JMenuItem("New");
//		menuItem.setMnemonic(KeyEvent.VK_N);
//		menuItem.addActionListener(this::newFile);
//		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, shortcutKeyCode));
//		fileMenu.add(menuItem);

		var menuItem = new JMenuItem("Open...");
		menuItem.setMnemonic(KeyEvent.VK_O);
		menuItem.addActionListener(this::openFile);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, shortcutKeyCode));
		fileMenu.add(menuItem);

		menuItem = new JMenuItem("Save as...");
		menuItem.setMnemonic(KeyEvent.VK_A);
		menuItem.addActionListener(this::saveFile);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, shortcutKeyCode | KeyEvent.SHIFT_DOWN_MASK));
		fileMenu.add(menuItem);

		// TODO don't show this on the Mac
		// FIXME this doesn't work all the time, maybe if other windows have been
		// opened?
//		menuItem = new JMenuItem("Exit");
//		menuItem.setMnemonic(KeyEvent.VK_X);
//		menuItem.addActionListener(event -> this.dispose());
//		fileMenu.addSeparator();
//		fileMenu.add(menuItem);

		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var tabs = new JTabbedPane();
		tabs.addTab("Accounts", accountsPanel);
		tabs.addTab("Budget", budgetPanel);

		layout.setVerticalGroup(layout.createParallelGroup().addComponent(tabs));
		layout.setHorizontalGroup(layout.createSequentialGroup().addComponent(tabs));
		pack();
	}

	// TODO this should prompt to save if the ledger is dirty
	private void openFile(ActionEvent event) {
		var dialog = new FileDialog(this);
		dialog.setFilenameFilter((dir, name) -> name.endsWith(".txt"));
		dialog.setVisible(true);
		var directory = dialog.getDirectory();
		var filename = dialog.getFile();
		if (directory == null || filename == null) {
			return;
		}

		var fullPath = Paths.get(directory, filename);
		try (var reader = Files.newBufferedReader(fullPath, StandardCharsets.UTF_8)) {
			var ledger = new Ledger();
			var ledgerReader = new LedgerReader(reader, fullPath.toString());
			ledgerReader.readAll(ledger);
			setLedger(ledger);
		} catch (IOException | LedgerException ex) {
			JOptionPane.showMessageDialog(this, "Error reading file.  " + ex.getLocalizedMessage(), "File Read Error",
					JOptionPane.ERROR_MESSAGE);
		}
		try (var reader = Files.newBufferedReader(fullPath, StandardCharsets.UTF_8)) {
			var ledger = new Ledger();
			var ledgerReader = new LedgerReader(reader, fullPath.toString());
			ledgerReader.readAll(ledger);
			setLedger(ledger);
		} catch (IOException | LedgerException ex) {
			JOptionPane.showMessageDialog(this, "Error reading file.  " + ex.getLocalizedMessage(), "File Read Error",
					JOptionPane.ERROR_MESSAGE);
		}
	}

	private void saveFile(ActionEvent event) {
		var dialog = new FileDialog(this, null, FileDialog.SAVE);
		dialog.setFilenameFilter((dir, name) -> name.endsWith(".txt"));
		dialog.setVisible(true);
		var dirname = dialog.getDirectory();
		var filename = dialog.getFile();
		if (dirname == null || filename == null) {
			return;
		}

		var filenameWithExt = filename;
		if (!filenameWithExt.endsWith(".txt")) {
			filenameWithExt += ".txt";
		}

		var fullPath = Paths.get(dirname, filenameWithExt);
		if (!Files.notExists(fullPath)) {
			JOptionPane.showMessageDialog(this, "Cowardly refusing to overwrite existing file.", "File Write Error",
					JOptionPane.ERROR_MESSAGE);
			return;
		}

		try (var writer = Files.newBufferedWriter(fullPath, StandardCharsets.UTF_8, StandardOpenOption.CREATE_NEW,
				StandardOpenOption.WRITE)) {
			var ledgerWriter = new LedgerWriter(writer);
			ledger.process(ledgerWriter);
		} catch (IOException | LedgerException ex) {
			JOptionPane.showMessageDialog(this, "Error writing file.  " + ex.getLocalizedMessage(), "File Write Error",
					JOptionPane.ERROR_MESSAGE);
		}
	}

	public void setLedger(Ledger ledger) throws LedgerException {
		this.ledger = ledger;
		accountsPanel.setLedger(ledger);
		budgetPanel.setLedger(ledger);
	}

}
