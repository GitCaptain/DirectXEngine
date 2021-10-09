#pragma once

#include "EventDispatcher.h"
#include "queue"

class EventListener {
public:
	EventListener() = default;
	virtual ~EventListener() = default;
	virtual void react(std::queue<EventType> events) const = 0;
};