package asdf;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;
import java.util.ArrayList;

import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.SwingUtilities;

@SuppressWarnings("serial")
public class SimpleGuiTests extends JFrame {

	private class Swatch extends JLabel {
		public Swatch(Color color) {
			setBackground(color);
			setMinimumSize(new Dimension(50, 50));
		}

		public boolean isOpaque() {
			return true;
		}
	}

	public SimpleGuiTests() {
		initUI();
	}

	private void initUI() {
		setTitle("Hello window");
		setSize(400, 300);
		setLocationRelativeTo(null);
		setDefaultCloseOperation(EXIT_ON_CLOSE);

		var button = new JButton("Click me!");
		button.setToolTipText("Click here for something neat.");
		button.setMnemonic(KeyEvent.VK_B);
		button.addActionListener((event) -> System.exit(0));

		var label = new JLabel("Just some text");
		label.setBackground(Color.BLUE);

		addMouseMotionListener(new MouseMotionAdapter() {
			public void mouseMoved(MouseEvent e) {
				label.setText("Mouse at " + e.getX() + " and " + e.getY());
			}
		});

		var swatches = new ArrayList<Swatch>();
		swatches.add(new Swatch(Color.RED));
		swatches.add(new Swatch(Color.BLACK));
		swatches.add(new Swatch(Color.BLUE));
		swatches.add(new Swatch(Color.GREEN));
		swatches.add(new Swatch(Color.CYAN));
		swatches.add(new Swatch(Color.DARK_GRAY));
		swatches.add(new Swatch(Color.GRAY));
		swatches.add(new Swatch(Color.LIGHT_GRAY));
//		createLayout(swatches.toArray(new Swatch[0]));
		createLayout(button, label, new Swatch(Color.BLUE));
	}

	private void createLayout(JComponent... components) {
		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);

		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		var horz = layout.createSequentialGroup();
		var vert = layout.createParallelGroup();
		for (var c : components) {
			horz = horz.addComponent(c);
			vert = vert.addComponent(c);
		}
		layout.setHorizontalGroup(horz);
		layout.setVerticalGroup(vert);
		pack();
	}

	private void createLayoutX(JComponent... components) {
		var pane = getContentPane();
		var layout = new GroupLayout(pane);
		pane.setLayout(layout);
		layout.setAutoCreateContainerGaps(true);
		layout.setAutoCreateGaps(true);

		layout.setHorizontalGroup(layout.createSequentialGroup()
				.addGroup(layout.createParallelGroup().addComponent(components[0]).addComponent(components[1])
						.addComponent(components[2]).addComponent(components[3]))
				.addGroup(layout.createParallelGroup().addComponent(components[4]).addComponent(components[5])
						.addComponent(components[6]).addComponent(components[7])));

		layout.setVerticalGroup(layout.createParallelGroup()
				.addGroup(layout.createSequentialGroup().addComponent(components[0]).addComponent(components[1])
						.addComponent(components[2]).addComponent(components[3]))
				.addGroup(layout.createSequentialGroup().addComponent(components[4]).addComponent(components[5])
						.addComponent(components[6]).addComponent(components[7])));
	}

	public static void main(String[] args) {
		SwingUtilities.invokeLater(() -> {
			SimpleGuiTests cp = new SimpleGuiTests();
			cp.setVisible(true);
		});
	}

}
