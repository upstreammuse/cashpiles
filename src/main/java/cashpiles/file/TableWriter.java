package cashpiles.file;

import java.io.BufferedWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import cashpiles.util.Comparisons;

class TableWriter {

	private List<List<String>> cells = new ArrayList<>();
	private List<String> currentRow;
	private List<Integer> maxWidths = new ArrayList<>();
	private BufferedWriter writer;

	TableWriter(BufferedWriter writer) {
		this.writer = writer;
	}

	void flush() throws IOException {
		if (currentRow != null) {
			newLine();
		}
		for (var row : cells) {
			assert (row.size() <= maxWidths.size());
			var sb = new StringBuilder();
			for (int i = 0; i < row.size(); i++) {
				if (maxWidths.get(i) > 0) {
					sb.append(String.format("%-" + maxWidths.get(i) + "s", row.get(i)));
				}
			}
			writer.write(sb.toString().stripTrailing());
			writer.newLine();
		}
		cells = new ArrayList<>();
		maxWidths = new ArrayList<>();
	}

	void newLine() {
		if (currentRow == null) {
			currentRow = new ArrayList<>();
		}
		cells.add(currentRow);
		currentRow = null;
	}

	// TODO there are almost certainly gotchas in here relating to unicode printable
	// character count vs string length
	void write(int index, String cell) {
		if (currentRow == null) {
			currentRow = new ArrayList<>();
		}
		while (currentRow.size() <= index) {
			currentRow.add("");
		}
		currentRow.set(index, cell);

		while (maxWidths.size() <= index) {
			maxWidths.add(0);
		}
		maxWidths.set(index, Comparisons.max(maxWidths.get(index), cell.length()));
	}

}
