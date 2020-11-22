package cashpiles.currency;

import static org.junit.jupiter.api.Assertions.*;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.time.LocalDate;
import java.time.Period;
import java.util.Random;

import org.junit.jupiter.api.Test;

import cashpiles.time.DateRange;

class AmountTests {

	@Test
	void testDistribute() {
		var random = new Random();
		var randomAmount = new Amount(new BigDecimal(BigInteger.probablePrime(64, new Random()), 2));
		var randomFrom = new DateRange(
				LocalDate.of(random.nextInt(3000) + 1, random.nextInt(12) + 1, random.nextInt(28) + 1),
				Period.ofDays(random.nextInt(1000) + 1));
		for (int i = 1; i < randomFrom.numberOfDays(); ++i) {
			var randomTo = new DateRange(randomFrom.startDate(), Period.ofDays(i));
			var remainingTo = randomTo.next();
			remainingTo = new DateRange(remainingTo.startDate(), Period.ofDays(randomFrom.numberOfDays() - i));
			var distAmount = randomAmount.distributeAndAdd(randomFrom, randomTo);
			var remAmount = randomAmount.distributeAndAdd(randomFrom, remainingTo);
			assertEquals(randomAmount, distAmount.add(remAmount));
		}

		randomAmount = new Amount(new BigDecimal(BigInteger.probablePrime(64, new Random()), 2).negate());
		for (int i = 1; i < randomFrom.numberOfDays(); ++i) {
			var randomTo = new DateRange(randomFrom.startDate(), Period.ofDays(i));
			var remainingTo = randomTo.next();
			remainingTo = new DateRange(remainingTo.startDate(), Period.ofDays(randomFrom.numberOfDays() - i));
			var distAmount = randomAmount.distributeAndAdd(randomFrom, randomTo);
			var remAmount = randomAmount.distributeAndAdd(randomFrom, remainingTo);
			assertEquals(randomAmount, distAmount.add(remAmount));
		}
	}

}
