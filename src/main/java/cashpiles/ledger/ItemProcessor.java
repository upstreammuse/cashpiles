package cashpiles.ledger;

public interface ItemProcessor {

	default void process(Account account) {
	}

	default void process(AccountTransactionEntry entry) {
	}

	default void process(Budget budget) {
	}

	default void process(CategoryTransactionEntry entry) {
	}

	default void process(CloseBudgetEntry entry) {
	}

	default void process(GoalBudgetEntry entry) {
	}

	default void process(IncomeBudgetEntry entry) {
	}

	default void process(LedgerItem item) {
	}

	default void process(ManualGoalBudgetEntry entry) {
	}

	default void process(OwnerTransactionEntry entry) {
	}

	default void process(ReserveBudgetEntry entry) {
	}

	default void process(RoutineBudgetEntry entry) {
	}

	default void process(Transaction xact) {
	}

	default void process(WithholdingBudgetEntry entry) {
	}

}
