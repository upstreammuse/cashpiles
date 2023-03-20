package cashpiles.model2;

import java.time.LocalDate;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

@SuppressWarnings("serial")
class ModelException extends Exception {

	static ModelException accountAlreadyOpen(String name) {
		return new ModelException("Account '" + name + "' is already open");
	}

	static ModelException accountNotExist(String name) {
		return new ModelException("Account '" + name + "' does not exist");
	}

	static ModelException accountNotOpen(String name) {
		return new ModelException("Account '" + name + "' is not open");
	}

	static ModelException accountTypeMismatch(String name) {
		return new ModelException("Cannot reopen account '" + name + "' with different type");
	}

	static ModelException budgetPeriodNotExist(LocalDate date) {
		return new ModelException("No budget period exists for date " + date);
	}

	static ModelException budgetPeriodOverlap(DateRange newDates, DateRange existingDates) {
		return new ModelException(
				"Budget period with dates " + newDates + " would overlap budget period with dates " + existingDates);
	}

	static ModelException identifierMismatch(String identifier, Model.IdentifierType establishedType,
			Model.IdentifierType mismatchedType) {
		return new ModelException("Cannot reuse name '" + identifier + "' for " + mismatchedType
				+ " when it is already used for " + establishedType);
	}

	static ModelException multipleBlankAmounts() {
		return new ModelException("Transaction cannot have multiple blank amounts");
	}

	static ModelException transactionUnbalanced(Amount accountBalance, Amount categoryBalance) {
		return new ModelException("Transaction account balance " + accountBalance + " and category balance "
				+ categoryBalance + " do not match");
	}

	static ModelException unknownIdentifier(String identifier) {
		return new ModelException("Unknown name '" + identifier + "'");
	}

	private ModelException(String text) {
		super(text);
	}

}
