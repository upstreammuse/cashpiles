package cashpiles.model;

import java.util.List;

public interface CategoriesView {

	CategoryView get(String name);

	List<String> names();

	int size();

}
