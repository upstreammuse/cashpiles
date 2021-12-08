package cashpiles.model;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.time.LocalDate;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

import cashpiles.currency.Amount;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.Transaction;
import cashpiles.ledger.TransactionException;
import cashpiles.ledger.UnbalancedTransaction;
import cashpiles.util.Lists;

// TODO what happens if the last category for a particular owner is closed? and
// what *should* happen?
public class Ledger {

	private final Map<String, Account> accounts = new HashMap<>();
	private final Map<LocalDate, List<LedgerItem>> items = new TreeMap<>();
	private final List<ActionListener> listeners = new ArrayList<>();

	// TODO this needs to deal with status flags to either throw errors if uncleared
	// items are present, and to automatically clear items that are reconciled
	// TODO right now this only handles creating statements from the pool of loose
	// transactions, and it doesn't figure out the deferrals, so adding a statement
	// before another existing statement will totally break
	// - needs to allow 'stealing' transactions from following statments that start
	// before the new end date
	// - needs to update deferrals for unstolen transactions that fit the date range
	// - needs mechanism to mark later statements dirty so they can revalidate
	// themselves
	// - needs to update deferrals for loose transactions before existing statements
	// that remain loose
	// - probably lots of other crazy gotchas that stem from the rather terrible
	// statement/balance concept
	public void add(AccountBalance balance) throws LedgerModelException {
		// make no permanent changes while exceptions can be thrown
		var account = accounts.get(balance.account());
		if (account == null) {
			throw LedgerModelException.forUnknown(balance);
		}

		var statement = new Statement(
				account.statements.isEmpty() ? new Amount() : Lists.lastOf(account.statements).balance());
		statement.closingDate = balance.date();
		List<TransactionParticle> newLooseTransactions = new ArrayList<>();
		for (var xact : account.looseTransactions) {
			// TODO need to add deferral to transactions
//			if (xact.deferral > 0) {
//				newLooseTransactions.add(xact);
//			} else {
			// only balance against loose transactions that happened before the statement
			if (xact.date().compareTo(balance.date()) <= 0) {
				statement.transactions.add(xact);
			} else {
				newLooseTransactions.add(xact);
			}
//			}
		}
		if (!statement.balance().equals(balance.amount())) {
			throw LedgerModelException.forUnbalanced(balance, statement.balance());
		}

		// make permanent changes without exceptions
		List<TransactionParticle> updatedLooseTransactions = new ArrayList<>();
		// TODO need to add deferral to transactions
//		for (var xact : newLooseTransactions) {
//			updatedLooseTransactions.add(xact.withDeferral(xact.deferral() - 1));
//		}
		account.looseTransactions = updatedLooseTransactions;
		account.statements.add(statement);
		insertEndOfDay(balance.date(), balance);
		notify("AccountBalance");
	}

	public void add(AccountCommand command) throws LedgerModelException {
		var account = accounts.get(command.account());
		switch (command.status()) {
		case ON_BUDGET, OFF_BUDGET -> {
			if (account != null) {
				if (account.status != command.status()) {
					throw LedgerModelException.forTypeChange(command);
				}
				throw LedgerModelException.forAlreadyOpen(command);
			}

			// no exceptions past this point
			account = new Account(command.date(), command.status());
			accounts.put(command.account(), account);
		}
		case CLOSED -> {
			if (account == null) {
				throw LedgerModelException.forAlreadyClosed(command);
			}
			if (!account.balance().equals(new Amount())) {
				throw LedgerModelException.forNonZeroClose(command);
			}

			// no exceptions past this point
			accounts.remove(command.account());
		}
		}
		insertEndOfDay(command.date(), command);
		notify("AccountCommand");
	}

	public void add(AccountTransactionEntry entry) throws LedgerModelException {
		var account = accounts.get(entry.account());
		if (account == null) {
			throw LedgerModelException.forUnknown(entry);
		}
		if (account.status != AccountCommand.Status.ON_BUDGET) {
			throw LedgerModelException.forBudgetNeeded(entry);
		}
		if (entry.parent().date().compareTo(account.startDate) < 0) {
			throw LedgerModelException.forTooEarly(entry, account.startDate);
		}

		// no exceptions past this point
		var particle = new TransactionParticle().withAmount(entry.amount()).withDate(entry.parent().date());
		account.looseTransactions.add(particle);
		insertEndOfDay(entry.parent().date(), entry);
		notify("AccountTransactionEntry");
	}

	public void add(TrackingTransactionEntry entry) throws LedgerModelException {
		if (entry.trackingAccount().isEmpty()) {
			return;
		}

		var account = accounts.get(entry.trackingAccount().get());
		if (account == null) {
			throw LedgerModelException.forUnknown(entry);
		}
		if (account.status != AccountCommand.Status.OFF_BUDGET) {
			throw LedgerModelException.forOffBudgetNeeded(entry);
		}
		if (entry.parent().date().compareTo(account.startDate) < 0) {
			throw LedgerModelException.forTooEarly(entry, account.startDate);
		}

		// no exceptions past this point
		var particle = new TransactionParticle().withAmount(entry.amount().negate()).withDate(entry.parent().date());
		account.looseTransactions.add(particle);
		insertEndOfDay(entry.parent().date(), entry);
		notify("TrackingTransactionEntry");
	}

	// TODO transactions arent exception safe because the transaction header could
	// be OK, but one of the sub-entries could fail, leaving a half-processed
	// transaction setup
	public boolean add(Transaction transaction) throws TransactionException {
		transaction.balance();
		insertEndOfDay(transaction.date(), transaction);
		notify("Transaction");
		return true;
	}

	public void add(UnbalancedTransaction transaction) throws LedgerModelException {
		var account = accounts.get(transaction.account());
		if (account == null) {
			throw LedgerModelException.forUnknown(transaction);
		}
		if (account.status != AccountCommand.Status.OFF_BUDGET) {
			throw LedgerModelException.forOffBudgetNeeded(transaction);
		}
		if (transaction.date().compareTo(account.startDate) < 0) {
			throw LedgerModelException.forTooEarly(transaction, account.startDate);
		}

		// no exceptions past this point
		var particle = new TransactionParticle().withAmount(transaction.amount()).withDate(transaction.date());
		account.looseTransactions.add(particle);
		insertEndOfDay(transaction.date(), transaction);
		notify("UnbalancedTransaction");
	}

	public void addListener(ActionListener listener) {
		listeners.add(listener);
	}

	public void process(ItemProcessor processor) {
		items.forEach((day, items) -> {
			items.forEach(item -> item.process(processor));
		});
		processor.finish();
	}

	private void insertEndOfDay(LocalDate date, LedgerItem item) {
		var dayItems = items.get(date);
		if (dayItems == null) {
			dayItems = new ArrayList<>();
			items.put(date, dayItems);
		}
		dayItems.add(item);
	}

	private void notify(String eventName) {
		for (var listener : listeners) {
			listener.actionPerformed(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, eventName));
		}
	}

}
