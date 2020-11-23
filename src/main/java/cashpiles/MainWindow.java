package cashpiles;

import java.awt.FileDialog;

import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.SwingUtilities;

@SuppressWarnings("serial")
class MainWindow extends JFrame {

	MainWindow() {
		initUi();
	}

	private void initUi() {
		setSize(320, 240);
		setTitle("CashPiles");

		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var openButton = new JButton("Open...");
		openButton.addActionListener(event -> {
			var dialog = new FileDialog(this);
			dialog.setFilenameFilter((dir, name) -> name.endsWith(".txt"));
			dialog.setVisible(true);
			var filename = dialog.getFile();
			if (filename != null) {
				System.out.println("trying to open " + dialog.getDirectory() + filename);
			} else {
				System.out.println("no file to open");
			}
		});

		layout.setVerticalGroup(layout.createSequentialGroup().addComponent(openButton));
		layout.setHorizontalGroup(layout.createSequentialGroup().addComponent(openButton));
		pack();
	}

	public static void main(String[] args) {
		SwingUtilities.invokeLater(() -> {
			var mainWindow = new MainWindow();
			mainWindow.setVisible(true);
		});
	}

}
