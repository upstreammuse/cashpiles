package asdf;

import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;

import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;

public class MenuTests extends JFrame {
	
	MenuTests() {
		initUI();
	}
	
	private void initUI() {
		System.setProperty("apple.laf.useScreenMenuBar", "true");
		var menuBar = new JMenuBar();
		var fileMenu = new JMenu("File");
		var item = new JMenuItem("Exit");
		item.addActionListener(e -> System.exit(0));
		item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_T, ActionEvent.META_MASK));
		fileMenu.add(new JMenuItem("OPen..."));
		fileMenu.addSeparator();
		fileMenu.add(item);
		menuBar.add(fileMenu);
		setJMenuBar(menuBar);
	}

	public static void main(String[] args) {
		SwingUtilities.invokeLater(() -> {
			var x = new MenuTests();
			x.setVisible(true);
		});
	}

}
