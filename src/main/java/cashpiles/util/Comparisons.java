package cashpiles.util;

import java.util.Objects;

public class Comparisons {

	public static <T extends Comparable<? super T>> T max(T a, T b) {
		Objects.requireNonNull(a);
		Objects.requireNonNull(b);
		if (a.compareTo(b) < 1) {
			return b;
		} else {
			return a;
		}
	}

	public static <T extends Comparable<? super T>> T min(T a, T b) {
		Objects.requireNonNull(a);
		Objects.requireNonNull(b);
		if (a.compareTo(b) < 1) {
			return a;
		} else {
			return b;
		}
	}

}
