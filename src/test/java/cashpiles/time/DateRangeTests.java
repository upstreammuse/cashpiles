package cashpiles.time;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import java.time.DateTimeException;
import java.time.LocalDate;
import java.time.Period;

import org.junit.jupiter.api.Test;

class DateRangeTests {

	@Test
	void testEndDate() {
		// find the ends of leap months
		var start = LocalDate.of(2020, 2, 1);
		var range = new DateRange(start, Period.ofMonths(1));
		assertEquals(range.endDate(), LocalDate.of(2020, 2, 29));

		// find the ends of leap years
		start = LocalDate.of(2020, 1, 1);
		range = new DateRange(start, Period.ofYears(1));
		assertEquals(range.endDate(), LocalDate.of(2020, 12, 31));

		// do sensible things over leap year boundaries
		start = LocalDate.of(2020, 2, 29);
		range = new DateRange(start, Period.ofYears(1));
		assertEquals(range.endDate(), LocalDate.of(2021, 2, 27));

		// make sure the end date matches a manually set one
		range = new DateRange(LocalDate.of(1947, 8, 17), LocalDate.of(2025, 3, 11));
		assertEquals(range.endDate(), LocalDate.of(2025, 3, 11));

		// make sure we fail bogus ranges
		try {
			new DateRange(start, Period.ofWeeks(-3));
			fail("No exception on negative date range");
		} catch (DateTimeException ex) {
		} catch (Exception ex) {
			fail("Wrong exception on negative date range");
		}
	}

	@Test
	void testEquals() {
		// ranges should be equal when constructed via days
		var start = LocalDate.of(2020, 5, 26);
		var range1 = new DateRange(start, Period.ofDays(55));
		var range2 = new DateRange(range1.startDate(), range1.endDate());
		assertEquals(range1, range2);

		// should not be equal when made differently, when covering same dates
		range1 = new DateRange(start, Period.ofMonths(2));
		range2 = new DateRange(range1.startDate(), range1.endDate());
		assertEquals(range1.startDate(), range2.startDate());
		assertEquals(range1.endDate(), range2.endDate());
		assertNotEquals(range1, range2);
	}

	@Test
	void testIntersect() {

		// |----|
		// |----|
		var start1 = LocalDate.of(2020, 3, 1);
		var end1 = LocalDate.of(2020, 3, 31);
		var range1 = new DateRange(start1, end1);
		assertEquals(range1.intersect(range1), range1);

		// |----|
		// ...|----|
		start1 = LocalDate.of(2020, 3, 1);
		end1 = LocalDate.of(2020, 3, 27);
		var start2 = LocalDate.of(2020, 3, 10);
		var end2 = LocalDate.of(2020, 4, 3);
		range1 = new DateRange(start1, end1);
		var range2 = new DateRange(start2, end2);
		var start3 = LocalDate.of(2020, 3, 10);
		var end3 = LocalDate.of(2020, 3, 27);
		var range3 = new DateRange(start3, end3);
		assertEquals(range1.intersect(range2), range3);
		assertEquals(range2.intersect(range1), range3);

		// |----|
		// .....|----|
		start1 = LocalDate.of(2020, 3, 1);
		end1 = LocalDate.of(2020, 3, 27);
		range1 = new DateRange(start1, end1);
		start2 = LocalDate.of(2020, 3, 27);
		end2 = LocalDate.of(2020, 4, 3);
		range2 = new DateRange(start2, end2);
		start3 = LocalDate.of(2020, 3, 27);
		end3 = LocalDate.of(2020, 3, 27);
		range3 = new DateRange(start3, end3);
		assertEquals(range1.intersect(range2), range3);
		assertEquals(range2.intersect(range1), range3);

		// |----|
		// .......|----|
		start1 = LocalDate.of(2020, 3, 1);
		end1 = LocalDate.of(2020, 3, 22);
		range1 = new DateRange(start1, end1);
		start2 = LocalDate.of(2020, 3, 27);
		end2 = LocalDate.of(2020, 4, 3);
		range2 = new DateRange(start2, end2);
		try {
			range1.intersect(range2);
			fail("No exception on null date intersection");
		} catch (DateTimeException ex) {
		} catch (Exception ex) {
			fail("Wrong exception on null date intersection");
		}

	}

	@Test
	void testNext() {
		var start1 = LocalDate.of(2020, 3, 1);
		var end1 = LocalDate.of(2020, 3, 10);
		var range1 = new DateRange(start1, end1);
		var range2 = range1.next();
		assertEquals(range2.startDate(), LocalDate.of(2020, 3, 11));
		assertEquals(range2.endDate(), LocalDate.of(2020, 3, 20));

		start1 = LocalDate.of(2020, 1, 1);
		range1 = new DateRange(start1, Period.ofMonths(1));
		range2 = range1.next();
		assertEquals(range2.startDate(), LocalDate.of(2020, 2, 1));
		assertEquals(range2.endDate(), LocalDate.of(2020, 2, 29));
		range2 = range2.next();
		assertEquals(range2.startDate(), LocalDate.of(2020, 3, 1));
		assertEquals(range2.endDate(), LocalDate.of(2020, 3, 31));
	}

	@Test
	void testNumberOfDays() {
		var start1 = LocalDate.of(2020, 1, 1);
		var range1 = new DateRange(start1, Period.ofYears(1));
		assertEquals(range1.numberOfDays(), 366);

		start1 = LocalDate.of(2000, 1, 1);
		range1 = new DateRange(start1, Period.ofYears(1));
		assertEquals(range1.numberOfDays(), 366);

		start1 = LocalDate.of(2004, 2, 15);
		range1 = new DateRange(start1, Period.ofMonths(1));
		assertEquals(range1.numberOfDays(), 29);
	}

	@Test
	void testOverlap() {
		// |----|
		// |----|
		var start1 = LocalDate.of(2020, 3, 1);
		var end1 = LocalDate.of(2020, 3, 31);
		var range1 = new DateRange(start1, end1);
		assertTrue(range1.overlaps(range1));

		// |----|
		// ...|----|
		start1 = LocalDate.of(2020, 3, 1);
		end1 = LocalDate.of(2020, 3, 27);
		var start2 = LocalDate.of(2020, 3, 10);
		var end2 = LocalDate.of(2020, 4, 3);
		range1 = new DateRange(start1, end1);
		var range2 = new DateRange(start2, end2);
		assertTrue(range1.overlaps(range2));

		// |----|
		// .....|----|
		start1 = LocalDate.of(2020, 3, 1);
		end1 = LocalDate.of(2020, 3, 27);
		range1 = new DateRange(start1, end1);
		start2 = LocalDate.of(2020, 3, 27);
		end2 = LocalDate.of(2020, 4, 3);
		range2 = new DateRange(start2, end2);
		assertTrue(range1.overlaps(range2));

		// |----|
		// .......|----|
		start1 = LocalDate.of(2020, 3, 1);
		end1 = LocalDate.of(2020, 3, 22);
		range1 = new DateRange(start1, end1);
		start2 = LocalDate.of(2020, 3, 27);
		end2 = LocalDate.of(2020, 4, 3);
		range2 = new DateRange(start2, end2);
		assertFalse(range1.overlaps(range2));
	}

}
