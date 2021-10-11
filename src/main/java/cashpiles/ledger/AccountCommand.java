package cashpiles.ledger;

import java.time.LocalDate;

public class AccountCommand extends LedgerItem {

	public enum Status {
		CLOSED, OFF_BUDGET, ON_BUDGET
	}

	private String account;
	private LocalDate date;
	private Status status;

	public AccountCommand(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public AccountCommand(AccountCommand other) {
		this(other.fileName, other.lineNumber, other.comment);
		account = other.account;
		date = other.date;
		status = other.status;
	}

	public String account() {
		return account;
	}

	public LocalDate date() {
		return date;
	}

	public Status status() {
		return status;
	}

	public AccountCommand withAccount(String account) {
		AccountCommand retval = new AccountCommand(this);
		retval.account = account;
		return retval;
	}

	public AccountCommand withDate(LocalDate date) {
		AccountCommand retval = new AccountCommand(this);
		retval.date = date;
		return retval;
	}

	public AccountCommand withStatus(Status status) {
		AccountCommand retval = new AccountCommand(this);
		retval.status = status;
		return retval;
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

}
