package cashpiles.budget;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.fail;

import java.math.BigDecimal;
import java.time.DateTimeException;
import java.time.LocalDate;
import java.time.Period;

import org.junit.jupiter.api.Test;

import cashpiles.currency.Amount;
import cashpiles.time.DateRange;

class GoalCalculatorTests {

	@Test
	void testAlreadyDone() {
		var gc = new GoalCalculator();
		gc = gc.withAllocationDates(new DateRange(LocalDate.of(2020, 2, 1), Period.ofMonths(1)));
		gc = gc.withGoalDates(new DateRange(LocalDate.of(2020, 1, 1), Period.ofMonths(1)));
		gc = gc.withGoalAmount(new Amount(new BigDecimal("100")));
		gc = gc.withGoalRepeat(false);
		assertEquals(gc.allocationAmount(), new Amount(new BigDecimal("0")));
		assertEquals(gc.completed(), true);
		try {
			gc.nextGoalDates();
			fail("No exception");
		} catch (DateTimeException ex) {
		} catch (Exception ex) {
			fail("Wrong exception");
		}
	}

	@Test
	void testAlreadyDoneRepeater() {
		var gc = new GoalCalculator();
		gc = gc.withAllocationDates(new DateRange(LocalDate.of(2020, 2, 1), Period.ofMonths(1)));
		gc = gc.withGoalDates(new DateRange(LocalDate.of(2020, 1, 1), Period.ofMonths(1)));
		gc = gc.withGoalAmount(new Amount(new BigDecimal("100")));
		gc = gc.withGoalRepeat(true);
		assertEquals(gc.allocationAmount(), new Amount(new BigDecimal("100")));
		assertEquals(gc.completed(), false);
		try {
			assertEquals(gc.nextGoalDates(), new DateRange(LocalDate.of(2020, 3, 1), Period.ofMonths(1)));
		} catch (Exception ex) {
			fail("Exception");
		}
	}

	@Test
	void testBasic() {
		var gc = new GoalCalculator();
		gc = gc.withAllocationDates(new DateRange(LocalDate.of(2020, 1, 15), Period.ofMonths(1)));
		gc = gc.withGoalDates(new DateRange(LocalDate.of(2020, 1, 1), Period.ofMonths(1)));
		gc = gc.withGoalAmount(new Amount(new BigDecimal("31")));
		gc = gc.withGoalRepeat(false);
		assertEquals(gc.allocationAmount(), new Amount(new BigDecimal("17")));
		assertEquals(gc.completed(), true);
		try {
			gc.nextGoalDates();
			fail("No exception");
		} catch (DateTimeException ex) {
		} catch (Exception ex) {
			fail("Wrong exception");
		}
	}

}
