package cashpiles.model;

public interface CategoriesView {

	CategoryView get(int index);

	String name(int index);

	int size();

}
