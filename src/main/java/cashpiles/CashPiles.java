package cashpiles;

import java.awt.Color;

import javax.swing.SwingUtilities;
import javax.swing.UIDefaults;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

class CashPiles {

	public static void main(String[] args) {
		SwingUtilities.invokeLater(() -> {
			try {
				UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
			} catch (ClassNotFoundException | InstantiationException | IllegalAccessException
					| UnsupportedLookAndFeelException ex) {
				// keep the default if the system L&F fails
			}
			System.setProperty("apple.laf.useScreenMenuBar", "true");
			UIDefaults defaults = UIManager.getLookAndFeelDefaults();
			if (defaults.get("Table.alternateRowColor") == null)
				defaults.put("Table.alternateRowColor", new Color(240, 240, 240));
			var mw = new MainWindow();
			mw.setVisible(true);
		});
	}

}
