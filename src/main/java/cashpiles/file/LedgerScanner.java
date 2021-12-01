package cashpiles.file;

import java.math.BigDecimal;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.text.ParsePosition;
import java.time.DateTimeException;
import java.time.LocalDate;
import java.time.Period;
import java.time.format.DateTimeFormatter;
import java.time.format.DateTimeParseException;
import java.util.NoSuchElementException;

import cashpiles.currency.Amount;

class LedgerScanner {

	// TODO this needs to be controllable by the user
	private DateTimeFormatter dateFormatter = DateTimeFormatter.ofPattern("M/d/yyyy");
	private int index = 0;
	private String source;

	LedgerScanner(String source) {
		this.source = source;
	}

	boolean hasNext() {
		skipWhitespace();
		return index < source.length();
	}

	boolean hasNextAmount() {
		var indexBackup = index;
		boolean success = false;
		try {
			nextAmount();
			success = true;
		} catch (NoSuchElementException ex) {
		}
		index = indexBackup;
		return success;
	}

	boolean hasNextDate() {
		var indexBackup = index;
		skipWhitespace();
		for (var workingIndex = index + 1; workingIndex <= source.length(); workingIndex++) {
			try {
				LocalDate.parse(source.subSequence(index, workingIndex), dateFormatter);
				index = indexBackup;
				return true;
			} catch (DateTimeException ex) {
			}
		}
		index = indexBackup;
		return false;
	}

	boolean hasNextIdentifier() {
		return hasNext();
	}

	boolean hasNextPercentage() {
		var indexBackup = index;
		skipWhitespace();
		var format = NumberFormat.getPercentInstance();
		var position = new ParsePosition(index);
		format.parse(source, position);
		index = indexBackup;
		return position.getErrorIndex() == -1;
	}

	boolean hasNextPeriod() {
		if (!hasNext()) {
			return false;
		}
		var indexBackup = index;
		var potential = next();
		try {
			Period.parse(potential);
			index = indexBackup;
			return true;
		} catch (DateTimeParseException ex) {
			index = indexBackup;
			return false;
		}
	}

	boolean hasNextSeparator() {
		return (index < source.length() && source.charAt(index) == '\t')
				|| (index + 1 < source.length() && source.charAt(index) == ' ' && source.charAt(index + 1) == ' ');
	}

	String next() {
		if (index >= source.length()) {
			throw new NoSuchElementException();
		}
		skipWhitespace();
		var builder = new StringBuilder();
		while (index < source.length() && !Character.isWhitespace(source.codePointAt(index))) {
			builder.appendCodePoint(source.codePointAt(index));
			index += Character.charCount(source.codePointAt(index));
		}
		return builder.toString();
	}

	Amount nextAmount() {

		var indexBackup = index;
		skipWhitespace();
		var resetPosition = new ParsePosition(index);
		index = indexBackup;

		var format = (DecimalFormat) NumberFormat.getCurrencyInstance();
		format.setParseBigDecimal(true);
		var position = new ParsePosition(resetPosition.getIndex());
		var number = (BigDecimal) format.parse(source, position);

		if (number == null || position.getErrorIndex() != -1) {
			format = (DecimalFormat) NumberFormat.getNumberInstance();
			format.setParseBigDecimal(true);
			position = new ParsePosition(resetPosition.getIndex());
			number = (BigDecimal) format.parse(source, position);
		}

		if (number == null || position.getErrorIndex() != -1) {
			throw new NoSuchElementException();
		}

		try {
			var amount = new Amount(number);
			index = position.getIndex();
			return amount;
		} catch (NumberFormatException ex) {
		}
		throw new NoSuchElementException();
	}

	LocalDate nextDate() {
		var indexBackup = index;
		skipWhitespace();

		LocalDate lastGoodDate = null;
		var lastGoodIndex = index;
		for (var workingIndex = index + 1; workingIndex <= source.length(); workingIndex++) {
			try {
				lastGoodDate = LocalDate.parse(source.subSequence(index, workingIndex), dateFormatter);
				lastGoodIndex = workingIndex;
			} catch (DateTimeException ex) {
				if (lastGoodDate != null) {
					break;
				}
			}
		}
		if (lastGoodDate != null) {
			index = lastGoodIndex;
			return lastGoodDate;
		}

		index = indexBackup;
		throw new NoSuchElementException();
	}

	String nextIdentifier() {
		var builder = new StringBuilder();
		while (hasNext()) {
			builder.append(next());
			if (hasNextSeparator()) {
				break;
			}
			while (index < source.length() && Character.isWhitespace(source.codePointAt(index))) {
				builder.appendCodePoint(source.codePointAt(index));
				index += Character.charCount(source.codePointAt(index));
			}
		}
		return builder.toString();
	}

	BigDecimal nextPercentage() {
		var indexBackup = index;
		skipWhitespace();

		var format = (DecimalFormat) NumberFormat.getPercentInstance();
		format.setParseBigDecimal(true);
		var position = new ParsePosition(index);
		var percentage = (BigDecimal) format.parse(source, position);
		if (position.getErrorIndex() == -1) {
			index = position.getIndex();
			return percentage;
		} else {
			index = indexBackup;
			throw new NoSuchElementException();
		}
	}

	Period nextPeriod() {
		var indexBackup = index;
		skipWhitespace();

		try {
			return Period.parse(next());
		} catch (DateTimeParseException ex) {
			index = indexBackup;
			throw new NoSuchElementException();
		}
	}

	private void skipWhitespace() {
		while (index < source.length() && Character.isWhitespace(source.codePointAt(index))) {
			index += Character.charCount(source.codePointAt(index));
		}
	}

}
