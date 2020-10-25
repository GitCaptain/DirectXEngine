#include "MouseClass.h"

using namespace Mouse;

void MouseClass::onLeftPressed(int x, int y) {
    leftDown = true;
    MouseEvent me{ MouseEvent::EventType::LPress, x, y };
    eventBuffer.push(me);
}

void MouseClass::onLeftReleased(int x, int y) {
    leftDown = false;
    MouseEvent me{ MouseEvent::EventType::LRelease, x, y };
    eventBuffer.push(me);
}

void MouseClass::onRightPressed(int x, int y) {
    rightDown = true;
    MouseEvent me{ MouseEvent::EventType::RPress, x, y };
    eventBuffer.push(me);
}

void MouseClass::onRightReleased(int x, int y) {
    rightDown = false;
    MouseEvent me{ MouseEvent::EventType::RRelease, x, y };
    eventBuffer.push(me);
}

void MouseClass::onMiddlePressed(int x, int y) {
    middleDown = true;
    MouseEvent me{ MouseEvent::EventType::MPress, x, y };
    eventBuffer.push(me);
}

void MouseClass::onMiddleReleased(int x, int y) {
    rightDown = false;
    MouseEvent me{ MouseEvent::EventType::MRelease, x, y };
    eventBuffer.push(me);
}

void MouseClass::onWheelUp(int x, int y) {
    eventBuffer.push(MouseEvent{ MouseEvent::EventType::WheelUp, x, y });
}

void MouseClass::onWheelDown(int x, int y) {
    eventBuffer.push(MouseEvent{ MouseEvent::EventType::WheelDown, x, y });
}

void MouseClass::onMouseMove(int x, int y) {
    this->x = x;
    this->y = y;
    eventBuffer.push(MouseEvent{ MouseEvent::EventType::Move, x, y });
}

bool MouseClass::isLeftDown() {
    return leftDown;
}

bool MouseClass::isMiddleDown() {
    return middleDown;
}

bool MouseClass::isRightDown() {
    return rightDown;
}

int MouseClass::getPosX() {
    return x;
}

int MouseClass::getPosY() {
    return y;
}

MousePoint MouseClass::getPos() {
    return MousePoint(x, y);
}

bool MouseClass::isEventBufferEmpty() {
    return eventBuffer.empty();
}

MouseEvent MouseClass::readEvent() {
    if (isEventBufferEmpty()) {
        return MouseEvent();
    }
    else {
        MouseEvent me{ eventBuffer.front() };
        eventBuffer.pop();
        return me;
    }    
}
