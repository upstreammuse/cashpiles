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
import cashpiles.ledger.BlankLine;
import cashpiles.ledger.ItemProcessor;
import cashpiles.ledger.LedgerItem;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.Transaction;
import cashpiles.ledger.TransactionException;
import cashpiles.ledger.UnbalancedTransaction;

// TODO what happens if the last category for a particular owner is closed? and what *should* happen?
// TODO transactions aren't exception safe because the transaction header could be OK, but one of the sub-entries could fail, leaving a half-processed transaction setup
public class Ledger {

	private final Map<String, Account> accounts = new HashMap<>();
	private final TreeMap<LocalDate, List<LedgerItem>> items = new TreeMap<>();
	private final List<ActionListener> listeners = new ArrayList<>();
	private List<LedgerItem> preDatedItems = new ArrayList<>();

	public void add(AccountBalance balance) throws LedgerModelException {
		var account = accounts.get(balance.account());
		if (account == null) {
			throw LedgerModelException.forUnknown(balance);
		}
		account = account.reconciled(balance);
		accounts.put(balance.account(), account);
		insertEndOfDay(balance.date(), balance);
		notify("AccountBalance");
	}

	public void add(AccountCommand command) throws LedgerModelException {
		var account = accounts.get(command.account());
		switch (command.status()) {
		case ON_BUDGET, OFF_BUDGET -> {
			if (account != null) {
				if (account.status() != command.status()) {
					throw LedgerModelException.forTypeChange(command);
				}
				throw LedgerModelException.forAlreadyOpen(command);
			}

			// no validation exceptions past this point
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

			// no validation exceptions past this point
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
		if (account.status() != AccountCommand.Status.ON_BUDGET) {
			throw LedgerModelException.forBudgetNeeded(entry);
		}
		if (entry.parent().date().compareTo(account.startDate()) < 0) {
			throw LedgerModelException.forTooEarly(entry, account.startDate());
		}

		// no exceptions past this point
		var particle = new TransactionParticle().withAmount(entry.amount()).withDate(entry.parent().date())
				.withStatus(entry.parent().status());
		account = account.withTransaction(particle);
		accounts.put(entry.account(), account);
		insertEndOfDay(entry.parent().date(), entry);
		notify("AccountTransactionEntry");
	}

	public void add(BlankLine blank) {
		insertEnd(blank);
		notify("BlankLine");
	}

	public void add(TrackingTransactionEntry entry) throws LedgerModelException {
		if (entry.trackingAccount().isEmpty()) {
			return;
		}

		var account = accounts.get(entry.trackingAccount().get());
		if (account == null) {
			throw LedgerModelException.forUnknown(entry);
		}
		if (account.status() != AccountCommand.Status.OFF_BUDGET) {
			throw LedgerModelException.forOffBudgetNeeded(entry);
		}
		if (entry.parent().date().compareTo(account.startDate()) < 0) {
			throw LedgerModelException.forTooEarly(entry, account.startDate());
		}

		// no exceptions past this point
		var particle = new TransactionParticle().withAmount(entry.amount().negate()).withDate(entry.parent().date())
				.withStatus(entry.parent().status());
		account = account.withTransaction(particle);
		accounts.put(entry.trackingAccount().get(), account);
		insertEndOfDay(entry.parent().date(), entry);
		notify("TrackingTransactionEntry");
	}

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
		if (account.status() != AccountCommand.Status.OFF_BUDGET) {
			throw LedgerModelException.forOffBudgetNeeded(transaction);
		}
		if (transaction.date().compareTo(account.startDate()) < 0) {
			throw LedgerModelException.forTooEarly(transaction, account.startDate());
		}

		// no exceptions past this point
		var particle = new TransactionParticle().withAmount(transaction.amount()).withDate(transaction.date())
				.withStatus(transaction.status());
		account = account.withTransaction(particle);
		accounts.put(transaction.account(), account);
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

	private void insertEnd(LedgerItem item) {
		if (items.isEmpty()) {
			assert (preDatedItems != null);
			preDatedItems.add(item);
		} else {
			insertEndOfDay(items.lastKey(), item);
		}
	}

	private void insertEndOfDay(LocalDate date, LedgerItem item) {
		var dayItems = items.get(date);
		if (dayItems == null) {
			dayItems = new ArrayList<>();
			items.put(date, dayItems);
		}
		if (preDatedItems != null) {
			for (var preDatedItem : preDatedItems) {
				dayItems.add(preDatedItem);
			}
			preDatedItems = null;
		}
		dayItems.add(item);
	}

	private void notify(String eventName) {
		for (var listener : listeners) {
			listener.actionPerformed(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, eventName));
		}
	}

}
