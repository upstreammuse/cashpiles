package cashpiles.model;

public interface StatementsView {

	AccountTransactionsView get(int index);

	int size();

}
