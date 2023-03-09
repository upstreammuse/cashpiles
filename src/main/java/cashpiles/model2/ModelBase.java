package cashpiles.model2;

class ModelBase implements Cloneable {

	// @Override
	protected ModelBase clone() {
		try {
			return (ModelBase) super.clone();
		} catch (CloneNotSupportedException ex) {
			throw new RuntimeException("Model clone failure");
		}
	}

}
