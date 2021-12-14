package cashpiles.ledger;

public interface ItemProcessor {

	default void finish() {
	}

	default void process(AccountBalance balance) {
		processGeneric(balance);
	}

	default void process(AccountCommand account) {
		processGeneric(account);
	}

	default void process(AccountTransactionEntry entry) {
		processGeneric(entry);
	}

	default void process(BlankLine line) {
		processGeneric(line);
	}

	default boolean process(Budget budget) {
		processGeneric(budget);
		return true;
	}

	default void process(CategoryTransactionEntry entry) {
		processGeneric(entry);
	}

	default void process(CloseBudgetEntry entry) {
		processGeneric(entry);
	}

	default void process(GoalBudgetEntry entry) {
		processGeneric(entry);
	}

	default void process(IncomeBudgetEntry entry) {
		processGeneric(entry);
	}

	default void process(ManualGoalBudgetEntry entry) {
		processGeneric(entry);
	}

	default void process(OwnerTransactionEntry entry) {
		processGeneric(entry);
	}

	default void process(ReserveBudgetEntry entry) {
		processGeneric(entry);
	}

	default void process(RoutineBudgetEntry entry) {
		processGeneric(entry);
	}

	default boolean process(Transaction transaction) {
		processGeneric(transaction);
		return true;
	}

	default void process(UnbalancedTransaction transaction) {
		processGeneric(transaction);
	}

	default void process(WithholdingBudgetEntry entry) {
		processGeneric(entry);
	}

	default void processGeneric(LedgerItem item) {
	}

}
