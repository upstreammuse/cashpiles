package cashpiles.model;

import java.util.TreeMap;

@SuppressWarnings("serial")
class CategoriesMap extends TreeMap<String, Category> implements CategoriesView {

	@Override
	public CategoryView get(int index) {
		return this.entrySet().stream().skip(index).findFirst().get().getValue();
	}

	@Override
	public String name(int index) {
		return this.entrySet().stream().skip(index).findFirst().get().getKey();
	}

}
