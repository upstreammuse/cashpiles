package cashpiles.model;

import java.util.List;
import java.util.TreeMap;
import java.util.stream.Collectors;

import cashpiles.currency.Amount;

@SuppressWarnings("serial")
class OwnerMap extends TreeMap<String, Amount> implements OwnersView {

	OwnerMap() {
		super();
	}

	OwnerMap(OwnerMap other) {
		super(other);
	}

	@Override
	public Amount get(String name) {
		return super.get(name);
	}

	@Override
	public List<String> names() {
		return keySet().stream().collect(Collectors.toList());
	}

}
