package cashpiles.ledger;

import java.time.LocalDate;

public class AccountCommand extends DatedLedgerItem {

	public enum Status {
		CLOSED, OFF_BUDGET, ON_BUDGET
	}

	private String account;
	private Status status;

	public AccountCommand(String fileName, int lineNumber, String comment) {
		super(fileName, lineNumber, comment);
	}

	public String account() {
		return account;
	}

	public Status status() {
		return status;
	}

	public AccountCommand withAccount(String account) {
		var retval = clone();
		retval.account = account;
		return retval;
	}

	public AccountCommand withStatus(Status status) {
		var retval = clone();
		retval.status = status;
		return retval;
	}

	@Override
	public AccountCommand withDate(LocalDate date) {
		return (AccountCommand) super.withDate(date);
	}

	@Override
	public void process(ItemProcessor processor) {
		processor.process(this);
	}

	@Override
	public AccountCommand clone() {
		return (AccountCommand) super.clone();
	}

}
