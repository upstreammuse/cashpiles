package cashpiles.ledger;

public interface ItemProcessor {

	default void process(AccountBalance balance) {
	}

	default void process(AccountCommand account) {
	}

	default void process(AccountTransactionEntry entry) {
	}

	default void process(BlankLine line) {
	}

	default boolean process(Budget budget) {
		return false;
	}

	default void process(CategoryTransactionEntry entry) {
	}

	default void process(CloseBudgetEntry entry) {
	}

	default void process(GoalBudgetEntry entry) {
	}

	default void process(IncomeBudgetEntry entry) {
	}

	default void process(ManualGoalBudgetEntry entry) {
	}

	default void process(OwnerTransactionEntry entry) {
	}

	default void process(ReserveBudgetEntry entry) {
	}

	default void process(RoutineBudgetEntry entry) {
	}

	default void process(TrackingTransactionEntry entry) {
	}

	default boolean process(Transaction transaction) {
		return false;
	}

	default void process(UnbalancedTransaction transaction) {
	}

	default void process(WithholdingBudgetEntry entry) {
	}

}
