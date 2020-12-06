#include "MouseEvent.h"

using namespace NMouse;

MouseEvent::MouseEvent(): eType(EventType::Invalid), x(0), y(0) {}

MouseEvent::MouseEvent(const EventType eType, const int x, const int y): eType(eType), x(x), y(y) {}

bool MouseEvent::isValid() const {
    return eType != EventType::Invalid;
}

MouseEvent::EventType MouseEvent::getType() const {
    return eType;
}

MousePoint MouseEvent::getPoint() const {
    return MousePoint(x, y);
}

int MouseEvent::getPosX() const {
    return x;
}

int MouseEvent::getPosY() const {
    return y;
}
