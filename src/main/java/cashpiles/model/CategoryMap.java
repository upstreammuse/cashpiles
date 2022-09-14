package cashpiles.model;

import java.util.List;
import java.util.TreeMap;
import java.util.stream.Collectors;

@SuppressWarnings("serial")
class CategoryMap extends TreeMap<String, Category> implements CategoriesView {

	CategoryMap() {
		super();
	}

	CategoryMap(CategoryMap other) {
		super(other);
	}

	@Override
	public CategoryView get(String name) {
		return super.get(name);
	}

	@Override
	public List<String> names() {
		return keySet().stream().collect(Collectors.toList());
	}

}
