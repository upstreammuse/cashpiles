package cashpiles.budget;

import static org.junit.jupiter.api.Assertions.*;

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
		gc.setAllocationDates(new DateRange(LocalDate.of(2020, 2, 1), Period.ofMonths(1)));
		gc.setGoalDates(new DateRange(LocalDate.of(2020, 1, 1), Period.ofMonths(1)));
		gc.setGoalAmount(new Amount(new BigDecimal("100")));
		gc.setGoalRepeat(false);
		assertEquals(gc.getAllocationAmount(), new Amount(new BigDecimal("0")));
		assertEquals(gc.getCompleted(), true);
		try {
			gc.getNextGoalDates();
			fail("No exception");
		} catch (DateTimeException ex) {
		} catch (Exception ex) {
			fail("Wrong exception");
		}
	}

	@Test
	void testAlreadyDoneRepeater() {
		var gc = new GoalCalculator();
		gc.setAllocationDates(new DateRange(LocalDate.of(2020, 2, 1), Period.ofMonths(1)));
		gc.setGoalDates(new DateRange(LocalDate.of(2020, 1, 1), Period.ofMonths(1)));
		gc.setGoalAmount(new Amount(new BigDecimal("100")));
		gc.setGoalRepeat(true);
		assertEquals(gc.getAllocationAmount(), new Amount(new BigDecimal("100")));
		assertEquals(gc.getCompleted(), false);
		try {
			assertEquals(gc.getNextGoalDates(), new DateRange(LocalDate.of(2020, 3, 1), Period.ofMonths(1)));
		} catch (Exception ex) {
			fail("Exception");
		}
	}

	@Test
	void testBasic() {
		var gc = new GoalCalculator();
		gc.setAllocationDates(new DateRange(LocalDate.of(2020, 1, 15), Period.ofMonths(1)));
		gc.setGoalDates(new DateRange(LocalDate.of(2020, 1, 1), Period.ofMonths(1)));
		gc.setGoalAmount(new Amount(new BigDecimal("31")));
		gc.setGoalRepeat(false);
		assertEquals(gc.getAllocationAmount(), new Amount(new BigDecimal("17")));
		assertEquals(gc.getCompleted(), true);
		try {
			gc.getNextGoalDates();
			fail("No exception");
		} catch (DateTimeException ex) {
		} catch (Exception ex) {
			fail("Wrong exception");
		}
	}

}
