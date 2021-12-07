package cashpiles.model;

import java.time.LocalDate;

import cashpiles.ledger.LedgerException;
import cashpiles.ledger.LedgerItem;

@SuppressWarnings("serial")
public class LedgerModelException extends LedgerException {

	static LedgerModelException forClosedAccount(String accountName) {
		// TODO Auto-generated method stub
		return null;
	}

	static LedgerModelException forExistingAccount(String accountName) {
		// TODO Auto-generated method stub
		return null;
	}

	static LedgerModelException forNonexistantAccount(String accountName) {
		// TODO Auto-generated method stub
		return null;
	}

	static LedgerModelException forNonzeroBalance(String accountName) {
		// TODO Auto-generated method stub
		return null;
	}

	static LedgerModelException forUnbalancedStatement() {
		// TODO Auto-generated method stub
		return null;
	}

	static LedgerModelException forUnorderedDate(LocalDate date) {
		// TODO Auto-generated method stub
		return null;
	}

	protected LedgerModelException(LedgerItem item, String message) {
		super(item, message);
	}

}
