#pragma once
#include "KeyboardEvent.h"

using namespace NKeyboard;

KeyboardEvent::KeyboardEvent() : eType(EventType::Invalid), key(0u) {}

KeyboardEvent::KeyboardEvent(const EventType eType, const key_type key): eType(eType), key(key) {}

bool KeyboardEvent::isPressEvent() const {
    return eType == EventType::Press;
}

bool KeyboardEvent::isReleaseEvent() const {
    return eType == EventType::Release;
}

bool KeyboardEvent::isValidEvent() const {
    return eType != EventType::Invalid;
}

unsigned char KeyboardEvent::getKeyCode() const {
    return key;
}
