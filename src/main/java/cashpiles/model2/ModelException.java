package cashpiles.model2;

@SuppressWarnings("serial")
class ModelException extends Exception {

	static ModelException accountAlreadyOpen(String name) {
		return new ModelException("Account '" + name + "' is already open");
	}

	static ModelException accountNotExist(String name) {
		return new ModelException("Account '" + name + "' does not exist");
	}

	static ModelException accountNotOpen(String name) {
		return new ModelException("Account '" + name + "' is not open");
	}

	static ModelException accountTypeMismatch(String name) {
		return new ModelException("Cannot reopen account '" + name + "' with different type");
	}

	private ModelException(String text) {
		super(text);
	}

}
