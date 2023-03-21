package cashpiles.model2;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

class Transaction extends ModelItem {

	/*
	 * TODO - an attempt to make sense of the transaction status loop that I don't
	 * understand any more. The cleared status of a XACT is written to ledger for
	 * distillation of entries when putting them in the file. when reading out, that
	 * status is used to populate the statuses of entries that had their status
	 * distilled away for matching. new entries get their own status, and overall
	 * xact status is recalculated as entries are added.
	 */

	enum Status {
		CLEARED, DISPUTED, PENDING
	}

	private LocalDate date;
	private List<TransactionEntry> entries = new ArrayList<>();
	private UUID id = UUID.randomUUID();
	private String payee;
	private Status status = Status.PENDING;

	Transaction(LocalDate date, String payee) {
		this.date = date;
		this.payee = payee;
	}

	Model addToModel(Model model) throws ModelException {
		var m = model;
		for (var entry : entries) {
			m = entry.addToModel(m);
		}
		return m;
	}

	Transaction balanced() throws ModelException {
		var balancer = new TransactionBalancer();

		// start the clone with an empty set of entries
		var xact = clone();
		xact.entries.clear();

		// use my entries to get balance setup, OK because entries don't change doing
		// this
		for (var entry : entries) {
			entry.addToBalance(balancer);
		}

		// make sure the entries are balanced
		if (!balancer.blankEntry && !balancer.accountBalance.equals(balancer.categoryBalance)) {
			throw ModelException.transactionUnbalanced(balancer.accountBalance, balancer.categoryBalance);
		}

		// create new entries from the balancer and give them to the clone
		for (var entry : entries) {
			xact = xact.withEntry(entry.getFromBalance(balancer));
		}
		return xact;
	}

	LocalDate date() {
		return date;
	}

	public UUID id() {
		return id;
	}

	String payee() {
		return payee;
	}

	Model removeFromModel(Model model) throws ModelException {
		for (var entry : entries) {
			model = entry.removeFromModel(model);
		}
		return model;
	}

	Status status() {
		return status;
	}

	Transaction withEntry(TransactionEntry entry) {
		var xact = clone();
		xact.entries.add(entry.withMergedStatus(xact.status).withParent(xact));
		if (xact.status == Status.PENDING) {
			xact.status = Status.CLEARED;
		}
		for (var e : xact.entries) {
			// TODO fix eclipse or find a better formatter
			xact.status = e.status().map(status -> switch (xact.status) {
			// if transaction is cleared
			case CLEARED -> switch (status) {
			// change to pending if the entry is pending, otherwise do not change
			case CLEARED -> Status.CLEARED;
			case DISPUTED -> Status.CLEARED;
			case PENDING -> Status.PENDING;
			};
			// if transaction is disputed
			case DISPUTED -> switch (status) {
			// do not change
			case CLEARED -> Status.DISPUTED;
			case DISPUTED -> Status.DISPUTED;
			case PENDING -> Status.DISPUTED;
			};
			// if transaction is pending
			case PENDING -> switch (status) {
			// do not change
			case CLEARED -> Status.PENDING;
			case DISPUTED -> Status.PENDING;
			case PENDING -> Status.PENDING;
			};
			}).orElse(xact.status);
		}
		return xact;
	}

	@Override
	protected Transaction clone() {
		var xact = (Transaction) super.clone();
		xact.entries = new ArrayList<>();
		for (var entry : entries) {
			xact = xact.withEntry(entry);
		}
		return xact;
	}

}
