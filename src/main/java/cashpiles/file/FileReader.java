package cashpiles.file;

import java.io.BufferedReader;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

import cashpiles.ledger.AccountCommand;
import cashpiles.ledger.AccountBalance;
import cashpiles.ledger.AccountTransactionEntry;
import cashpiles.ledger.BlankLine;
import cashpiles.ledger.Budget;
import cashpiles.ledger.CategoryTransactionEntry;
import cashpiles.ledger.CloseBudgetEntry;
import cashpiles.ledger.GoalBudgetEntry;
import cashpiles.ledger.IncomeBudgetEntry;
import cashpiles.ledger.Ledger;
import cashpiles.ledger.ManualGoalBudgetEntry;
import cashpiles.ledger.OwnerTransactionEntry;
import cashpiles.ledger.ReserveBudgetEntry;
import cashpiles.ledger.RoutineBudgetEntry;
import cashpiles.ledger.TrackingTransactionEntry;
import cashpiles.ledger.Transaction;
import cashpiles.ledger.TransactionEntry;
import cashpiles.ledger.TransactionException;
import cashpiles.ledger.UnbalancedTransaction;
import cashpiles.ledger.WithholdingBudgetEntry;
import cashpiles.time.DateRange;

public class FileReader extends java.io.FileReader {

	public enum IdentifierType {
		ACCOUNT, CATEGORY, OWNER
	};

	private Budget activeBudget;
	private Ledger activeLedger;
	private Transaction activeTransaction;
	private final String fileName;
	private final Map<String, IdentifierType> identifiers = new HashMap<>();
	private int lineNumber = 0;

	public FileReader(String fileName) throws IOException {
		super(fileName, StandardCharsets.UTF_8);
		this.fileName = fileName;
	}

	public Ledger readAll() throws IOException, UnknownIdentifierException, IdentifierMismatchException,
			InvalidContentException, TransactionException {
		activeLedger = new Ledger();
		var buf = new BufferedReader(this);
		for (var line = buf.readLine(); line != null; line = buf.readLine()) {
			lineNumber++;
			processLine(line);
		}
		if (activeBudget != null) {
			activeLedger.add(activeBudget);
			activeBudget = null;
		}
		if (activeTransaction != null) {
			activeTransaction.balance();
			activeLedger.add(activeTransaction);
			activeTransaction = null;
		}
		return activeLedger;
	}

	private void processLine(String line) throws UnknownIdentifierException, IdentifierMismatchException,
			InvalidContentException, TransactionException {
		var comment = "";
		var split = line.indexOf(';');

		if (split != -1) {
			comment = line.substring(split + 1);
			line = line.substring(0, split);
		}

		line = line.stripTrailing();
		comment = comment.strip();

		if (activeBudget != null && !processBudgetClose(line, comment) && !processBudgetGoal(line, comment)
				&& !processBudgetIncome(line, comment) && !processBudgetReserve(line, comment)
				&& !processBudgetRoutine(line, comment) && !processBudgetWithholding(line, comment)
				&& !processBudgetManualGoal(line, comment)) {
			activeLedger.add(activeBudget);
			activeBudget = null;
		}

		if (activeTransaction != null && !processTransactionLine(line, comment)
				&& !processTransactionTrackingLine(line, comment)) {
			activeTransaction.balance();
			activeLedger.add(activeTransaction);
			activeTransaction = null;
		}

		if (activeBudget == null && activeTransaction == null && !processAccount(line, comment)
				&& !processBudget(line, comment) && !processTransaction(line, comment)
				&& !processUnbalancedTransaction(line, comment) && !processAccountBalance(line, comment)
				&& !processBlank(line, comment)) {
			throw new InvalidContentException(fileName, lineNumber, line);
		}
	}

	private boolean processAccount(String line, String comment) throws IdentifierMismatchException {
		var account = new AccountCommand(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (!scanner.hasNextDate()) {
			return false;
		}
		account.date = scanner.nextDate();

		if (!scanner.hasNext()) {
			return false;
		}
		switch (scanner.next()) {
		case "on-budget" -> {
			account.status = AccountCommand.Status.ON_BUDGET;
		}
		case "off-budget" -> {
			account.status = AccountCommand.Status.OFF_BUDGET;
		}
		case "close" -> {
			account.status = AccountCommand.Status.CLOSED;
		}
		default -> {
			return false;
		}
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		account.name = scanner.nextIdentifier();
		verifySetIdentifier(account.name, IdentifierType.ACCOUNT);

		if (scanner.hasNext()) {
			return false;
		}
		activeLedger.add(account);
		return true;
	}

	private boolean processAccountBalance(String line, String comment)
			throws UnknownIdentifierException, IdentifierMismatchException {
		var balance = new AccountBalance(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (!scanner.hasNextDate()) {
			return false;
		}
		balance.date = scanner.nextDate();

		if (!scanner.hasNext() || !scanner.next().equals("balance")) {
			return false;
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		balance.account = scanner.nextIdentifier();
		verifyIdentifier(balance.account, IdentifierType.ACCOUNT);

		if (!scanner.hasNextAmount()) {
			return false;
		}
		balance.amount = scanner.nextAmount();

		if (scanner.hasNext()) {
			return false;
		}
		activeLedger.add(balance);
		return true;
	}

	private boolean processBlank(String line, String comment) {
		var blank = new BlankLine(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (scanner.hasNext()) {
			return false;
		}
		activeLedger.add(blank);
		return true;
	}

	private boolean processBudget(String line, String comment) {
		var budget = new Budget(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (scanner.hasNextDate()) {
			budget.date = scanner.nextDate();
		} else {
			return false;
		}

		if (!scanner.hasNext() || !scanner.next().equals("budget")) {
			return false;
		}

		if (scanner.hasNextPeriod()) {
			budget.period = scanner.nextPeriod();
		} else {
			return false;
		}

		if (scanner.hasNext()) {
			return false;
		}
		activeBudget = budget;
		return true;
	}

	private boolean processBudgetClose(String line, String comment)
			throws UnknownIdentifierException, IdentifierMismatchException {
		var close = new CloseBudgetEntry(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (!scanner.hasNextSeparator()) {
			return false;
		}

		if (!scanner.hasNext() || !scanner.next().equals("close")) {
			return false;
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		close.category = scanner.nextIdentifier();
		verifyIdentifier(close.category, IdentifierType.CATEGORY);

		if (scanner.hasNext()) {
			return false;
		}
		activeBudget.entries.add(close);
		return true;
	}

	private boolean processBudgetGoal(String line, String comment)
			throws UnknownIdentifierException, IdentifierMismatchException {
		var goal = new GoalBudgetEntry(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (!scanner.hasNextSeparator()) {
			return false;
		}

		if (!scanner.hasNext() || !scanner.next().equals("goal")) {
			return false;
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		goal.name = scanner.nextIdentifier();
		verifySetIdentifier(goal.name, IdentifierType.CATEGORY);

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		goal.owner = scanner.nextIdentifier();
		verifySetIdentifier(goal.owner, IdentifierType.OWNER);

		if (!scanner.hasNextAmount()) {
			return false;
		}
		goal.amount = scanner.nextAmount();

		if (scanner.hasNextDate()) {
			goal.dates = new DateRange(activeBudget.date, scanner.nextDate());
		} else if (scanner.hasNextPeriod()) {
			goal.dates = new DateRange(activeBudget.date, scanner.nextPeriod());
		} else {
			return false;
		}

		goal.repeat = false;
		if (scanner.hasNext()) {
			if (!scanner.next().equals("repeat")) {
				return false;
			}
			goal.repeat = true;
		}

		if (scanner.hasNext()) {
			return false;
		}
		activeBudget.entries.add(goal);
		return true;
	}

	private boolean processBudgetIncome(String line, String comment)
			throws UnknownIdentifierException, IdentifierMismatchException {
		var income = new IncomeBudgetEntry(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (!scanner.hasNextSeparator()) {
			return false;
		}

		if (!scanner.hasNext() || !scanner.next().equals("income")) {
			return false;
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		income.name = scanner.nextIdentifier();
		verifySetIdentifier(income.name, IdentifierType.CATEGORY);

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		income.owner = scanner.nextIdentifier();
		verifySetIdentifier(income.owner, IdentifierType.OWNER);

		if (scanner.hasNext()) {
			return false;
		}
		activeBudget.entries.add(income);
		return true;
	}

	private boolean processBudgetManualGoal(String line, String comment) throws IdentifierMismatchException {
		var goal = new ManualGoalBudgetEntry(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (!scanner.hasNextSeparator()) {
			return false;
		}

		if (!scanner.hasNext() || !scanner.next().equals("goal")) {
			return false;
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		goal.name = scanner.nextIdentifier();
		verifySetIdentifier(goal.name, IdentifierType.CATEGORY);

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		goal.owner = scanner.nextIdentifier();
		verifySetIdentifier(goal.owner, IdentifierType.OWNER);

		if (scanner.hasNext()) {
			return false;
		}
		activeBudget.entries.add(goal);
		return true;
	}

	private boolean processBudgetReserve(String line, String comment)
			throws UnknownIdentifierException, IdentifierMismatchException {
		var reserve = new ReserveBudgetEntry(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (!scanner.hasNextSeparator()) {
			return false;
		}

		if (!scanner.hasNext() || !scanner.next().equals("reserve")) {
			return false;
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		reserve.name = scanner.nextIdentifier();
		verifySetIdentifier(reserve.name, IdentifierType.CATEGORY);

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		reserve.owner = scanner.nextIdentifier();
		verifySetIdentifier(reserve.owner, IdentifierType.OWNER);

		if (!scanner.hasNextPercentage()) {
			return false;
		}
		reserve.percentage = scanner.nextPercentage();

		if (scanner.hasNext()) {
			return false;
		}
		activeBudget.entries.add(reserve);
		return true;
	}

	private boolean processBudgetRoutine(String line, String comment)
			throws UnknownIdentifierException, IdentifierMismatchException {
		var routine = new RoutineBudgetEntry(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (!scanner.hasNextSeparator()) {
			return false;
		}

		if (!scanner.hasNext() || !scanner.next().equals("routine")) {
			return false;
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		routine.name = scanner.nextIdentifier();
		verifySetIdentifier(routine.name, IdentifierType.CATEGORY);

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		routine.owner = scanner.nextIdentifier();
		verifySetIdentifier(routine.owner, IdentifierType.OWNER);

		if (scanner.hasNext()) {
			return false;
		}
		activeBudget.entries.add(routine);
		return true;
	}

	private boolean processBudgetWithholding(String line, String comment)
			throws UnknownIdentifierException, IdentifierMismatchException {
		var withholding = new WithholdingBudgetEntry(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (!scanner.hasNextSeparator()) {
			return false;
		}

		if (!scanner.hasNext() || !scanner.next().equals("withholding")) {
			return false;
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		withholding.name = scanner.nextIdentifier();
		verifySetIdentifier(withholding.name, IdentifierType.CATEGORY);

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		withholding.owner = scanner.nextIdentifier();
		verifySetIdentifier(withholding.owner, IdentifierType.OWNER);

		if (scanner.hasNext()) {
			return false;
		}
		activeBudget.entries.add(withholding);
		return true;
	}

	private boolean processTransaction(String line, String comment) {
		var xact = new Transaction(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (!scanner.hasNextDate()) {
			return false;
		}
		xact.date = scanner.nextDate();

		if (!scanner.hasNext()) {
			return false;
		}
		try {
			xact.status = switch (scanner.next()) {
			case "*" -> Transaction.Status.CLEARED;
			case "!" -> Transaction.Status.DISPUTED;
			case "?" -> Transaction.Status.PENDING;
			default -> throw new IllegalArgumentException();
			};
		} catch (IllegalArgumentException ex) {
			return false;
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		xact.payee = scanner.nextIdentifier();

		if (scanner.hasNextAmount()) {
			scanner.nextAmount();
		}

		if (scanner.hasNext()) {
			return false;
		}
		activeTransaction = xact;
		return true;
	}

	private boolean processTransactionLine(String line, String comment) throws UnknownIdentifierException {
		TransactionEntry entry;
		var scanner = new Scanner(line);

		if (!scanner.hasNextSeparator()) {
			return false;
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		var ident = scanner.nextIdentifier();
		switch (identifierType(ident)) {

		case ACCOUNT -> {
			entry = new AccountTransactionEntry(fileName, lineNumber, comment);
			((AccountTransactionEntry) entry).account = ident;

		}
		case CATEGORY -> {
			entry = new CategoryTransactionEntry(fileName, lineNumber, comment);
			((CategoryTransactionEntry) entry).category = ident;

		}
		case OWNER -> {

			entry = new OwnerTransactionEntry(fileName, lineNumber, comment);
			((OwnerTransactionEntry) entry).owner = ident;

		}
		default -> {
			return false;
		}

		}

		if (scanner.hasNextAmount()) {
			entry.amount = scanner.nextAmount();
		}

		if (scanner.hasNext()) {
			return false;
		}
		activeTransaction.entries.add(entry);
		return true;
	}

	private boolean processTransactionTrackingLine(String line, String comment) throws UnknownIdentifierException {

		TrackingTransactionEntry entry;
		var scanner = new Scanner(line);

		if (!scanner.hasNextSeparator()) {
			return false;
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		var ident = scanner.nextIdentifier();
		switch (identifierType(ident)) {

		case CATEGORY -> {
			entry = new CategoryTransactionEntry(fileName, lineNumber, comment);
			((CategoryTransactionEntry) entry).category = ident;

		}
		case OWNER -> {

			entry = new OwnerTransactionEntry(fileName, lineNumber, comment);
			((OwnerTransactionEntry) entry).owner = ident;

		}
		default -> {
			return false;
		}
		}

		if (scanner.hasNextAmount()) {
			entry.amount = scanner.nextAmount();
		} else if (scanner.hasNextIdentifier()) {
			entry.trackingAccount = Optional.of(scanner.nextIdentifier());
			if (scanner.hasNextAmount()) {
				entry.amount = scanner.nextAmount();
			}
		}

		if (scanner.hasNext()) {
			return false;
		}

		activeTransaction.entries.add(entry);
		return true;

	}

	private boolean processUnbalancedTransaction(String line, String comment)
			throws UnknownIdentifierException, IdentifierMismatchException {
		var xact = new UnbalancedTransaction(fileName, lineNumber, comment);
		var scanner = new Scanner(line);

		if (!scanner.hasNextDate()) {
			return false;
		}
		xact.date = scanner.nextDate();

		if (!scanner.hasNext()) {
			return false;
		}
		try {
			xact.status = switch (scanner.next()) {
			case "*" -> Transaction.Status.CLEARED;
			case "!" -> Transaction.Status.DISPUTED;
			case "?" -> Transaction.Status.PENDING;
			default -> throw new IllegalArgumentException();
			};
		} catch (IllegalArgumentException ex) {
			return false;
		}

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		xact.account = scanner.nextIdentifier();
		verifySetIdentifier(xact.account, IdentifierType.ACCOUNT);

		if (!scanner.hasNextIdentifier()) {
			return false;
		}
		xact.payee = scanner.nextIdentifier();

		if (!scanner.hasNextAmount()) {
			return false;
		}
		xact.amount = scanner.nextAmount();

		if (scanner.hasNext()) {
			return false;
		}
		activeLedger.add(xact);
		return true;
	}

	private IdentifierType identifierType(String identifier) throws UnknownIdentifierException {
		if (!identifiers.containsKey(identifier)) {
			throw new UnknownIdentifierException(identifier);
		}
		return identifiers.get(identifier);
	}

	private void verifyIdentifier(String identifier, IdentifierType type)
			throws UnknownIdentifierException, IdentifierMismatchException {
		if (!identifiers.containsKey(identifier)) {
			throw new UnknownIdentifierException(identifier, type);
		}
		if (identifiers.get(identifier) != type) {
			throw new IdentifierMismatchException(identifier, type, identifiers.get(identifier));
		}
	}

	private void verifySetIdentifier(String identifier, IdentifierType type) throws IdentifierMismatchException {
		if (identifiers.containsKey(identifier) && identifiers.get(identifier) != type) {
			throw new IdentifierMismatchException(identifier, type, identifiers.get(identifier));
		}
		identifiers.put(identifier, type);
	}

}
