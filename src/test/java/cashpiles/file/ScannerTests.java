package cashpiles.file;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.math.BigDecimal;
import java.time.LocalDate;
import java.time.Month;
import java.time.Period;

import org.junit.jupiter.api.Test;

import cashpiles.currency.Amount;

// TODO rework date-based tests to account for user-specified formatting
class ScannerTests {

	@Test
	void testAccount() {
		var input = "4/11/2012 on-budget My New Account";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextDate());
		assertEquals(scanner.nextDate(), LocalDate.of(2012, 4, 11));
		assertTrue(scanner.hasNext());
		assertEquals(scanner.next(), "on-budget");
		assertTrue(scanner.hasNextIdentifier());
		assertEquals(scanner.nextIdentifier(), "My New Account");
		assertFalse(scanner.hasNext());
	}

	@Test
	void testAccountBalance() {
		var input = "2/29/2000 balance My New Account  $1,512.51";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextDate());
		assertEquals(scanner.nextDate(), LocalDate.of(2000, 2, 29));
		assertTrue(scanner.hasNext());
		assertEquals(scanner.next(), "balance");
		assertTrue(scanner.hasNextIdentifier());
		assertEquals(scanner.nextIdentifier(), "My New Account");
		assertTrue(scanner.hasNextAmount());
		assertEquals(scanner.nextAmount(), new Amount(new BigDecimal("1512.51")));
		assertFalse(scanner.hasNext());
	}

	@Test
	void testBudget() {
		var input = "11/26/1999 budget P2W";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextDate());
		assertEquals(scanner.nextDate(), LocalDate.of(1999, 11, 26));
		assertTrue(scanner.hasNext());
		assertEquals(scanner.next(), "budget");
		assertTrue(scanner.hasNextPeriod());
		assertEquals(scanner.nextPeriod(), Period.ofWeeks(2));
		assertFalse(scanner.hasNext());
	}

	@Test
	void testGoal() {
		var input = "  goal    Emergency Fund  Car Maintenance Buffer  $1,000.00 12/31/2019";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextSeparator());
		assertTrue(scanner.hasNext());
		assertEquals(scanner.next(), "goal");
		assertTrue(scanner.hasNextIdentifier());
		assertEquals(scanner.nextIdentifier(), "Emergency Fund");
		assertTrue(scanner.hasNextIdentifier());
		assertEquals(scanner.nextIdentifier(), "Car Maintenance Buffer");
		assertTrue(scanner.hasNextAmount());
		assertEquals(scanner.nextAmount(), new Amount(new BigDecimal("1000")));
		assertTrue(scanner.hasNextDate());
		assertEquals(scanner.nextDate(), LocalDate.of(2019, Month.DECEMBER, 31));
		assertFalse(scanner.hasNext());
	}

	@Test
	void testReservePercent() {
		var input = "  reserve A category  42% An owner";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextSeparator());
		assertTrue(scanner.hasNext());
		assertEquals(scanner.next(), "reserve");
		assertTrue(scanner.hasNextIdentifier());
		assertEquals(scanner.nextIdentifier(), "A category");
		assertTrue(scanner.hasNextPercentage());
		assertEquals(scanner.nextPercentage(), new BigDecimal(".42"));
		assertTrue(scanner.hasNextIdentifier());
		assertEquals(scanner.nextIdentifier(), "An owner");
		assertFalse(scanner.hasNext());
	}

	@Test
	void testCurrency() {
		var input = "-498718923182412.31";
		var inputCurr = "-$498718923182412.31";
		var scanner = new LedgerScanner(inputCurr);
		assertTrue(scanner.hasNextAmount());
		assertEquals(scanner.nextAmount(), new Amount(new BigDecimal(input)));

		input = "42";
		scanner = new LedgerScanner("$" + input);
		assertTrue(scanner.hasNextAmount());
		assertEquals(scanner.nextAmount(), new Amount(new BigDecimal(input)));
	}

	@Test
	void testDateBad() {
		var input = "1-1-10";
		var scanner = new LedgerScanner(input);
		assertFalse(scanner.hasNextDate());
	}

	@Test
	void testDateGood() {
		var input = "1/1/1910";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextDate());
		assertEquals(scanner.nextDate(), LocalDate.of(1910, 1, 1));
	}

	@Test
	void testDateLeading() {
		var input = " \t\n1/1/1910";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextDate());
		assertEquals(scanner.nextDate(), LocalDate.of(1910, 1, 1));
	}

	@Test
	void testGeneralBad() {
		var input = "\n\n\n\t\t    ";
		var scanner = new LedgerScanner(input);
		assertFalse(scanner.hasNext());
	}

	@Test
	void testGeneralGood() {
		var input = "some text";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNext());
		assertEquals(scanner.next(), "some");
	}

	@Test
	void testGeneralLeading() {
		var input = "\n\n\t\t   some text";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNext());
		assertEquals(scanner.next(), "some");
		assertTrue(scanner.hasNext());
		assertEquals(scanner.next(), "text");
		assertFalse(scanner.hasNext());
	}

	@Test
	void testIdentifier() {
		var input = "\t  \n  My Bank  Something Else     A third thing\tA fourth";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextIdentifier());
		assertEquals(scanner.nextIdentifier(), "My Bank");
		assertTrue(scanner.hasNextIdentifier());
		assertEquals(scanner.nextIdentifier(), "Something Else");
		assertTrue(scanner.hasNextIdentifier());
		assertEquals(scanner.nextIdentifier(), "A third thing");
		assertTrue(scanner.hasNextIdentifier());
		assertEquals(scanner.nextIdentifier(), "A fourth");
		assertFalse(scanner.hasNextIdentifier());
	}

	@Test
	void testPercent() {
		var input = "   34%  ";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextPercentage());
		assertEquals(scanner.nextPercentage(), new BigDecimal(".34"));
	}

	@Test
	void testPeriod() {
		var input = "p148d";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextPeriod());
		assertEquals(scanner.nextPeriod(), Period.ofDays(148));

		input = "p17w";
		scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextPeriod());
		assertEquals(scanner.nextPeriod(), Period.ofWeeks(17));

		input = "p58m";
		scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextPeriod());
		assertEquals(scanner.nextPeriod(), Period.ofMonths(58));

		input = "p10000y";
		scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextPeriod());
		assertEquals(scanner.nextPeriod(), Period.ofYears(10000));
	}

	@Test
	void testSeparator() {
		var input = "  ";
		var scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextSeparator());

		input = "\t";
		scanner = new LedgerScanner(input);
		assertTrue(scanner.hasNextSeparator());

		input = " \n \n ";
		scanner = new LedgerScanner(input);
		assertFalse(scanner.hasNextSeparator());
	}

}
