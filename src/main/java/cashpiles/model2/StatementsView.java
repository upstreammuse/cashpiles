package cashpiles.model2;

import java.time.LocalDate;

public interface StatementsView {

	LocalDate earliestReconcileDate();

	AccountTransactionsView get(int index);

	int size();

}
