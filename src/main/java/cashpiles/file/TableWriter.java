package cashpiles.file;

import java.io.BufferedWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import cashpiles.util.Comparisons;

class TableWriter {

	public enum Alignment {
		ALIGN_LEFT, ALIGN_RIGHT
	}

	private static class Cell {
		Alignment align = Alignment.ALIGN_LEFT;
		String value = "";
	}

	private List<List<Cell>> cells = new ArrayList<>();
	private List<Cell> currentRow;
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
					String aligner = switch (row.get(i).align) {
					case ALIGN_LEFT -> "-";
					case ALIGN_RIGHT -> "";
					};
					sb.append(String.format("%" + aligner + maxWidths.get(i) + "s", row.get(i).value));
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

	void write(int index, String cell) {
		write(index, cell, Alignment.ALIGN_LEFT);
	}

	// TODO there are almost certainly gotchas in here relating to unicode printable
	// character count vs string length
	public void write(int index, String content, Alignment align) {
		if (currentRow == null) {
			currentRow = new ArrayList<>();
		}
		while (currentRow.size() <= index) {
			currentRow.add(new Cell());
		}
		var cell = currentRow.get(index);
		cell.align = align;
		cell.value = content;
		currentRow.set(index, cell);

		while (maxWidths.size() <= index) {
			maxWidths.add(0);
		}
		maxWidths.set(index, Comparisons.max(maxWidths.get(index), content.length()));
	}

}
