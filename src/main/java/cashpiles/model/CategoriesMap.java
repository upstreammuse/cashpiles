package cashpiles.model;

import java.util.TreeSet;

@SuppressWarnings("serial")
class CategoriesMap extends TreeSet<String> {

	public String name(int index) {
		return this.stream().skip(index).findFirst().get();
	}

}
