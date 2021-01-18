package cashpiles.ui.budget;

import java.util.function.Consumer;

import javax.swing.JTable;

import cashpiles.budget.BudgetPeriod;
import cashpiles.currency.Amount;

class BudgetPeriodWindowController {

	private Consumer<Amount> categoryActivityConsumer;
	private Consumer<Amount> categoryAllocationConsumer;
	private Consumer<Amount> categoryBalanceConsumer;
	private final BudgetPeriodCategoryModel categoryModel = new BudgetPeriodCategoryModel();
	private final BudgetPeriodOwnerModel ownerModel = new BudgetPeriodOwnerModel();
	private Consumer<Amount> ownerBalanceConsumer;

	void forCategoryTable(JTable table) {
		table.setModel(categoryModel);
	}

	void forOwnerTable(JTable table) {
		table.setModel(ownerModel);
	}

	void onCategoryActivity(Consumer<Amount> consumer) {
		categoryActivityConsumer = consumer;
	}

	void onCategoryAllocation(Consumer<Amount> consumer) {
		categoryAllocationConsumer = consumer;
	}

	void onCategoryBalance(Consumer<Amount> consumer) {
		categoryBalanceConsumer = consumer;
	}

	void onOwnerBalance(Consumer<Amount> consumer) {
		ownerBalanceConsumer = consumer;
	}

	void setPeriod(BudgetPeriod period) {
		categoryModel.setPeriod(period);
		ownerModel.setPeriod(period);
		recalcTotals();
	}

	private void emitCategoryActivity(Amount amount) {
		if (categoryActivityConsumer != null) {
			categoryActivityConsumer.accept(amount);
		}
	}

	private void emitCategoryAllocation(Amount amount) {
		if (categoryAllocationConsumer != null) {
			categoryAllocationConsumer.accept(amount);
		}
	}

	private void emitCategoryBalance(Amount amount) {
		if (categoryBalanceConsumer != null) {
			categoryBalanceConsumer.accept(amount);
		}
	}

	private void emitOwnerBalance(Amount amount) {
		if (ownerBalanceConsumer != null) {
			ownerBalanceConsumer.accept(amount);
		}
	}

	private void recalcTotals() {
		var categoryAllocation = new Amount();
		var categoryActivity = new Amount();
		var categoryBalance = new Amount();
		for (int i = 0; i < categoryModel.getRowCount(); ++i) {
			categoryAllocation = categoryAllocation.add((Amount) categoryModel.getValueAt(i, 2));
			categoryActivity = categoryActivity.add((Amount) categoryModel.getValueAt(i, 3));
			categoryBalance = categoryBalance.add((Amount) categoryModel.getValueAt(i, 4));
		}
		emitCategoryActivity(categoryActivity);
		emitCategoryAllocation(categoryAllocation);
		emitCategoryBalance(categoryBalance);

		var ownerBalance = new Amount();
		for (int i = 0; i < ownerModel.getRowCount(); ++i) {
			ownerBalance = ownerBalance.add((Amount) ownerModel.getValueAt(i, 1));
		}
		emitOwnerBalance(ownerBalance);
	}

}
