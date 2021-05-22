package cashpiles.budget;

import java.util.ArrayList;
import java.util.List;

import cashpiles.currency.Amount;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.time.DateRange;

public abstract class BudgetCategory {

	protected final String name;
	protected final String owner;
	protected final Amount startBalance;
	protected final List<CategoryTransactionEntry> transactions = new ArrayList<>();

	protected BudgetCategory(String name, String owner, Amount startBalance) {
		this.name = name;
		this.owner = owner;
		this.startBalance = startBalance;
	}

	void addTransaction(CategoryTransactionEntry transaction) {
		transactions.add(transaction);
	}

	void close(CloseBudgetEntry entry) throws BudgetReconfigureException {
		if (!transactions.isEmpty()) {
			throw BudgetReconfigureException.forCategoryClosure(entry);
		}
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

	public abstract BudgetCategory next(DateRange dates);

	public abstract String typeString();

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
