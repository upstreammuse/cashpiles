package cashpiles.file;

import java.awt.FileDialog;
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
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import cashpiles.ledger.Ledger;
import cashpiles.ledger.TransactionException;

@SuppressWarnings("serial")
public class FileWindow extends JFrame {

	private Ledger ledger = new Ledger();

	public FileWindow() {
		initUI();
	}

	private void initUI() {
		setDefaultCloseOperation(DISPOSE_ON_CLOSE);

		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var menuBar = new JMenuBar();
		setJMenuBar(menuBar);

		var fileMenu = new JMenu("File");
		fileMenu.setMnemonic(KeyEvent.VK_F);
		menuBar.add(fileMenu);

		var menuItem = new JMenuItem("New");
		menuItem.setMnemonic(KeyEvent.VK_N);
		menuItem.addActionListener(this::newFile);
		fileMenu.add(menuItem);

		menuItem = new JMenuItem("Open...");
		menuItem.setMnemonic(KeyEvent.VK_O);
		menuItem.addActionListener(this::openFile);
		fileMenu.add(menuItem);

		menuItem = new JMenuItem("Save as...");
		menuItem.setMnemonic(KeyEvent.VK_A);
		menuItem.addActionListener(this::saveFile);
		fileMenu.add(menuItem);

		menuItem = new JMenuItem("Exit");
		menuItem.setMnemonic(KeyEvent.VK_X);
		menuItem.addActionListener(event -> this.dispose());
		fileMenu.addSeparator();
		fileMenu.add(menuItem);

		pack();
	}

	private void newFile(ActionEvent event) {
		// FIXME prompt save
		ledger = new Ledger();
	}

	private void openFile(ActionEvent event) {
		var dialog = new FileDialog(this);
		dialog.setFilenameFilter((dir, name) -> name.endsWith(".txt"));
		dialog.setVisible(true);
		var filename = dialog.getFile();
		if (filename != null) {
			newFile(event);
			try (var reader = new LedgerReader(dialog.getDirectory() + filename)) {
				ledger = reader.readAll();
			} catch (IOException | UnknownIdentifierException | IdentifierMismatchException | TransactionException
					| InvalidContentException ex) {
				JOptionPane.showMessageDialog(this, "Error reading file.  " + ex.getLocalizedMessage(),
						"File Read Error", JOptionPane.ERROR_MESSAGE);
			}
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

	public static void main(String[] args) {
		SwingUtilities.invokeLater(() -> {
			try {
				UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
			} catch (ClassNotFoundException | InstantiationException | IllegalAccessException
					| UnsupportedLookAndFeelException ex) {
				// keep the default if the system L&F fails
			}
			var window = new FileWindow();
			window.setVisible(true);
		});
	}

}
