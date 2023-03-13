package cashpiles.time;

import java.math.BigInteger;
import java.time.DateTimeException;
import java.time.LocalDate;
import java.time.Period;
import java.time.temporal.ChronoUnit;

import cashpiles.util.Comparisons;

public class DateRange {

	private final boolean hasExplicitPeriod;
	private final Period period;
	private final LocalDate startDate;

	public DateRange(LocalDate startDate, LocalDate endDate) {
		sortDates(startDate, endDate);
		this.startDate = startDate;
		period = Period.ofDays(countDays(startDate, endDate));
		hasExplicitPeriod = false;
	}

	public DateRange(LocalDate startDate, Period period) {
		this.startDate = startDate;
		this.period = period;
		sortDates(startDate, endDate());
		hasExplicitPeriod = true;
	}

	public boolean contains(LocalDate date) {
		return startDate.compareTo(date) <= 0 && endDate().compareTo(date) >= 0;
	}

	public final LocalDate endDate() {
		return startDate.plus(period).minusDays(1);
	}

	@Override
	public boolean equals(Object other) {
		if (other == null) {
			return false;
		}
		if (!getClass().equals(other.getClass())) {
			return false;
		}
		return ((DateRange) other).startDate.equals(startDate) && ((DateRange) other).period.equals(period);
	}

	public boolean hasExplicitPeriod() {
		return hasExplicitPeriod;
	}

	@Override
	public int hashCode() {
		return startDate.hashCode() ^ period.hashCode();
	}

	public DateRange intersect(DateRange other) {
		var newStart = Comparisons.max(startDate, other.startDate);
		var newEnd = Comparisons.min(endDate(), other.endDate());
		sortDates(newStart, newEnd);
		return new DateRange(newStart, newEnd);
	}

	public DateRange next() {
		return new DateRange(startDate.plus(period), period);
	}

	public int numberOfDays() {
		return countDays(startDate, endDate());
	}

	public boolean overlaps(DateRange other) {
		var newStart = Comparisons.max(startDate, other.startDate);
		var newEnd = Comparisons.min(endDate(), other.endDate());
		return newStart.compareTo(newEnd) <= 0;
	}

	public Period period() {
		return period;
	}

	public LocalDate startDate() {
		return startDate;
	}

	public String toString() {
		return startDate.toString() + "+" + period + "->" + endDate().toString();
	}

	private static int countDays(LocalDate startDate, LocalDate endDate) {
		var days = ChronoUnit.DAYS.between(startDate, endDate) + 1;
		return BigInteger.valueOf(days).intValueExact();
	}

	private static void sortDates(LocalDate startDate, LocalDate endDate) {
		if (endDate.compareTo(startDate) < 0) {
			throw new DateTimeException("End date is before start date");
		}
	}

}
