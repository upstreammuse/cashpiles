package cashpiles.util;

import java.util.List;

public class Lists {
	
	public static <T> T lastOf(List<T> list) {
		return list.get(list.size() - 1);
	}

}
