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
import javax.swing.KeyStroke;

import cashpiles.account.AccountsWindow;
import cashpiles.budget.ui.BudgetWindow;
import cashpiles.file.LedgerReader;
import cashpiles.file.LedgerWriter;
import cashpiles.ledger.Ledger;
import cashpiles.ledger.LedgerException;

// TODO look into the Preferences class to store user settings in a platform-matching way
@SuppressWarnings("serial")
class MainWindow extends JFrame {

	private final Ledger ledger;

	MainWindow(Ledger ledger) {
		this.ledger = ledger;
		initUi();
	}

	private void initUi() {
		setSize(320, 240);
		setTitle("CashPiles");
		// FIXME this should be a dispose on close, but every window needs that set
		// before it works properly
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

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
		menuItem = new JMenuItem("Exit");
		menuItem.setMnemonic(KeyEvent.VK_X);
		menuItem.addActionListener(event -> this.dispose());
		fileMenu.addSeparator();
		fileMenu.add(menuItem);

		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var xactPanel = new TransactionPanel(ledger);

		layout.setVerticalGroup(layout.createParallelGroup().addComponent(xactPanel));
		layout.setHorizontalGroup(layout.createSequentialGroup().addComponent(xactPanel));
		pack();
	}

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
			var ledgerReader = new LedgerReader(reader, fullPath.toString());
			// TODO this should go in an action thread to not hang the gui? would complicate
			// the thread safety of the ledger, however, and need to be synchronized to each
			// ledger change it made
			ledgerReader.readAll(ledger);
			var accountWindow = new AccountsWindow();
			ledger.process(accountWindow);
			accountWindow.setVisible(true);
			var budgetWindow = new BudgetWindow();
			ledger.process(budgetWindow);
			budgetWindow.setVisible(true);
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
		} catch (IOException ex) {
			JOptionPane.showMessageDialog(this, "Error writing file.  " + ex.getLocalizedMessage(), "File Write Error",
					JOptionPane.ERROR_MESSAGE);
		}
	}

}
