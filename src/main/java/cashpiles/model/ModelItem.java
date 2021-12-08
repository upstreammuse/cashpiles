package cashpiles.model;

class ModelItem implements Cloneable {

	@Override
	public ModelItem clone() {
		try {
			return (ModelItem) super.clone();
		} catch (CloneNotSupportedException ex) {
			return null;
		}
	}

}
