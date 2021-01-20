package cashpiles.currency;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.math.RoundingMode;
import java.text.NumberFormat;
import java.time.DateTimeException;
import java.time.Period;

import cashpiles.time.DateRange;

public class Amount {

	private final BigDecimal value;

	public Amount() {
		value = BigDecimal.ZERO;
	}

	public Amount(BigDecimal value) {
		if (value.scale() > NumberFormat.getCurrencyInstance().getMaximumFractionDigits()) {
			throw new NumberFormatException("Too many decimal places");
		}
		this.value = value;
	}

	public Amount add(Amount other) {
		return new Amount(value.add(other.value));
	}

	public Amount distributeAndAdd(DateRange fromDates, DateRange toDates) {
		// make sure we have something to do
		DateRange resultDates;
		try {
			resultDates = fromDates.intersect(toDates);
		} catch (DateTimeException ex) {
			return new Amount();
		}

		// determine amount per fromDate and number of +1 days
		var division = value.unscaledValue().divideAndRemainder(BigInteger.valueOf(fromDates.numberOfDays()));
		division[0] = division[0].abs();
		division[1] = division[1].abs();

		// number of +1 days in result
		var numHigherDates = 0;
		try {
			var fromHigherDates = new DateRange(fromDates.startDate(), Period.ofDays(division[1].intValueExact()));
			var resultHigherDates = fromHigherDates.intersect(toDates);
			numHigherDates = resultHigherDates.numberOfDays();
		} catch (DateTimeException ex) {
		}

		// unscaled value is number of result days * per date value + number of higher
		// days in result
		var unscaledResult = division[0].multiply(BigInteger.valueOf(resultDates.numberOfDays()))
				.add(BigInteger.valueOf(numHigherDates));

		// return to negative value if original value was negative
		if (isNegative()) {
			unscaledResult = unscaledResult.negate();
		}

		return new Amount(new BigDecimal(unscaledResult, value.scale()));

	}

	@Override
	public boolean equals(Object other) {
		if (other == null) {
			return false;
		}
		if (!getClass().equals(other.getClass())) {
			return false;
		}
		return value.compareTo(((Amount) other).value) == 0;
	}

	@Override
	public int hashCode() {
		return value.setScale(NumberFormat.getCurrencyInstance().getMaximumFractionDigits()).hashCode();
	}

	public boolean isNegative() {
		return value.compareTo(BigDecimal.ZERO) < 0;
	}

	public Amount negate() {
		return new Amount(value.negate());
	}

	public Amount percentage(BigDecimal percentage) {
		return new Amount(value.multiply(percentage)
				.setScale(NumberFormat.getCurrencyInstance().getMaximumFractionDigits(), RoundingMode.HALF_EVEN));
	}

	public Amount times(int x) {
		return new Amount(value.multiply(new BigDecimal(x)));
	}

	public String toString() {
		return NumberFormat.getCurrencyInstance().format(value);
	}

}
