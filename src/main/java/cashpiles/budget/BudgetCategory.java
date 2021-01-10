package cashpiles.budget;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.time.DateRange;

public abstract class BudgetCategory implements Cloneable {

	protected final String name;
	protected final String owner;
	protected final Map<String, Amount> owners;
	protected final Amount startBalance;
	protected final List<CategoryTransactionEntry> transactions = new ArrayList<>();

	public BudgetCategory(String name, Amount startBalance, Map<String, Amount> owners, String owner) {
		this.name = name;
		this.owner = owner;
		this.owners = owners;
		this.startBalance = startBalance;
		if (!owners.containsKey(owner)) {
			owners.put(owner, new Amount());
		}
	}

	void addTransaction(CategoryTransactionEntry transaction) {
		transactions.add(transaction);
	}

	@Override
	public abstract BudgetCategory clone();

	void close() {
		if (!transactions.isEmpty()) {
			throw new RuntimeException("Cannot close category that has activity");
		}
		owners.put(owner, owners.get(owner).add(getBalance()));
	}

	public boolean exceedsDates(DateRange dates) {
		return transactions.stream().anyMatch(
				t -> dates.startDate().compareTo(t.parent.date) > 0 || dates.endDate().compareTo(t.parent.date) < 0);
	}

	public Amount getActivity() {
		return transactions.stream().map(t -> t.amount).reduce(new Amount(), Amount::add);
	}

	public abstract Amount getAllocation();

	public Amount getBalance() {
		return startBalance.add(getAllocation()).add(getActivity());
	}

	public void link(BudgetCategory category) {
		// stop here to avoid infinite dispatch
	}

	public void link(String name, IncomeCategory category) {
		// stop here to avoid infinite dispatch
	}

	public void link(String name, ReserveCategory category) {
		// stop here to avoid infinite dispatch
	}

	void unlink(BudgetCategory category) {
		// stop here to avoid infinite dispatch
	}

	void unlink(String name, IncomeCategory category) {
		// stop here to avoid infinite dispatch
	}

	void unlink(String name, ReserveCategory category) {
		// stop here to avoid infinite dispatch
	}

}
