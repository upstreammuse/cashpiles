package cashpiles.model2;

class ModelItem implements Cloneable {

	// @Override
	protected ModelItem clone() {
		try {
			return (ModelItem) super.clone();
		} catch (CloneNotSupportedException ex) {
			throw new RuntimeException("Model clone failure");
		}
	}

}
