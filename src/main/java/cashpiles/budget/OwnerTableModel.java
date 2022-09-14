package cashpiles.budget;

import java.util.Map;
import java.util.TreeMap;
import java.util.stream.Collectors;

import javax.swing.table.AbstractTableModel;

import cashpiles.currency.Amount;
import cashpiles.model.CategoriesView;
import cashpiles.model.OwnersView;

@SuppressWarnings("serial")
class OwnerTableModel extends AbstractTableModel {

	private static final String[] headers = { "Owner", "Unallocated Balance" };

	private final Map<String, Amount> owners = new TreeMap<>();

	OwnerTableModel() {
	}

	OwnerTableModel(CategoriesView categories, OwnersView owners) {
		for (var owner : owners.names()) {
			this.owners.put(owner, owners.get(owner));
		}
		for (var categoryName : categories.names()) {
			var category = categories.get(categoryName);
			var owner = category.owner();
			this.owners.put(owner, this.owners.get(owner).add(category.lifetimeAllocation().negate()));
		}
	}

	@Override
	public int getColumnCount() {
		return headers.length;
	}

	@Override
	public String getColumnName(int col) {
		return headers[col];
	}

	@Override
	public int getRowCount() {
		return owners.size();
	}

	@Override
	public Object getValueAt(int row, int col) {
		var o = owners.entrySet().stream().collect(Collectors.toList());
		return switch (col) {
		case 0 -> o.get(row).getKey();
		case 1 -> o.get(row).getValue();
		default -> throw new IllegalArgumentException("Unexpected value: " + col);
		};
	}

}
