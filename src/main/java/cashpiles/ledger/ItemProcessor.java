package cashpiles.ledger;

public interface ItemProcessor {

	default void finish() throws LedgerException {
	}

	default void process(AccountBalance balance) throws LedgerException {
		processGeneric(balance);
	}

	default void process(AccountCommand account) throws LedgerException {
		processGeneric(account);
	}

	default void process(AccountTransactionEntry entry) throws LedgerException {
		processGeneric(entry);
	}

	default void process(BlankLine line) throws LedgerException {
		processGeneric(line);
	}

	default boolean process(Budget budget) throws LedgerException {
		processGeneric(budget);
		return false;
	}

	default void process(CategoryTransactionEntry entry) throws LedgerException {
		processGeneric(entry);
	}

	default void process(CloseBudgetEntry entry) throws LedgerException {
		processGeneric(entry);
	}

	default void process(GoalBudgetEntry entry) throws LedgerException {
		processGeneric(entry);
	}

	default void process(IncomeBudgetEntry entry) throws LedgerException {
		processGeneric(entry);
	}

	default void process(ManualGoalBudgetEntry entry) throws LedgerException {
		processGeneric(entry);
	}

	default void process(OwnerTransactionEntry entry) throws LedgerException {
		processGeneric(entry);
	}

	default void process(ReserveBudgetEntry entry) throws LedgerException {
		processGeneric(entry);
	}

	default void process(RoutineBudgetEntry entry) throws LedgerException {
		processGeneric(entry);
	}

	default boolean process(Transaction transaction) throws LedgerException {
		processGeneric(transaction);
		return false;
	}

	default void process(UnbalancedTransaction transaction) throws LedgerException {
		processGeneric(transaction);
	}

	default void process(WithholdingBudgetEntry entry) throws LedgerException {
		processGeneric(entry);
	}

	default void processGeneric(LedgerItem item) throws LedgerException {
	}

}
