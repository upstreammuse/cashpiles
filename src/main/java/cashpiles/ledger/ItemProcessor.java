package cashpiles.ledger;

public interface ItemProcessor {

	default void process(Budget budget) {
	}

	default void process(CategoryTransactionEntry entry) {
	}

	default void process(CloseBudgetEntry entry) {
	}

	default void process(GoalBudgetEntry entry) {
	}

	default void process(LedgerItem item) {
	}

	default void process(OwnerTransactionEntry entry) {
	}

	default void process(Transaction xact) {
	}

}
