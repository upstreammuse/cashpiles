package cashpiles.model;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class Ledger {

	private List<LedgerItem> items = new ArrayList<>();
	private List<ActionListener> listeners = new ArrayList<>();

	public boolean add(LedgerItem item) {
		item = Objects.requireNonNull(item, "Cannot add null item to ledger");
		var retval = items.add(item);
		notifyAdd();
		return retval;
	}

	public void addListener(ActionListener listener) {
		listeners.add(listener);
	}

	public void clear() {
		items.clear();
		notifyClear();
	}

	public int indexOf(LedgerItem item) {
		return items.indexOf(item);
	}

	public void process(ItemProcessor processor) {
		for (var item : items) {
			item.process(processor);
		}
		processor.finish();
	}

	public LedgerItem set(int index, LedgerItem item) {
		item = Objects.requireNonNull(item, "Cannot add null item to ledger");
		var retval = items.set(index, item);
		notifySet();
		return retval;
	}

	private void notifyAdd() {
		for (var listener : listeners) {
			listener.actionPerformed(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "add"));
		}
	}

	private void notifyClear() {
		for (var listener : listeners) {
			listener.actionPerformed(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "clear"));
		}
	}

	private void notifySet() {
		for (var listener : listeners) {
			listener.actionPerformed(new ActionEvent(this, ActionEvent.ACTION_PERFORMED, "set"));
		}
	}

}
