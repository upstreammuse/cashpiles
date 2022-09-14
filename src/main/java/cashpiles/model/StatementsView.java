package cashpiles.model;

import java.time.LocalDate;

public interface StatementsView {

	LocalDate earliestReconcileDate();

	AccountTransactionsView get(int index);

	int size();

}
