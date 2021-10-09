#pragma once

#include "vector"
#include "queue"

class EventDispatcher {
protected:
	EventDispatcher() = default;
	virtual ~EventDispatcher() = default;
};

enum class EventType {
};

class EventListener;

class HIDDispatcher {
public:

public:
	void subscribe(const EventListener& listener);
	//void unsubscribe(const EventListener& listener);
	void notify();
	void reset();

private:
	std::vector<const EventListener*> listeners;
	std::queue<EventType> events;
};