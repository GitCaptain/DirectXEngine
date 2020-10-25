#include "MouseEvent.h"

using namespace Mouse;

MouseEvent::MouseEvent(): eType(EventType::Invalid), x(0), y(0) {}

MouseEvent::MouseEvent(const EventType eType, const int x, const int y): eType(eType), x(x), y(y) {}

bool MouseEvent::isValid() const {
    return eType != EventType::Invalid;
}

MouseEvent::EventType MouseEvent::GetType() const {
    return eType;
}

MousePoint MouseEvent::GetPoint() const {
    return MousePoint(x, y);
}

int MouseEvent::GetPosX() const {
    return x;
}

int MouseEvent::GetPosY() const {
    return y;
}
