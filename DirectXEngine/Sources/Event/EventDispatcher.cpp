#include "EventDispatcher.h"
#include "EventListener.h"

void HIDDispatcher::subscribe(const EventListener& listener) {
	listeners.push_back(&listener);
}

void HIDDispatcher::notify() {
	for (const EventListener *listener : listeners) {
		listener->react(events);
	}
}

void HIDDispatcher::reset() {
	std::queue<EventType> q;
	events.swap(q);
}
