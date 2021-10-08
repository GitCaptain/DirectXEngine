#include "Mouse.h"

using namespace HID;

void Mouse::onLeftPressed(int x, int y) {
    leftDown = true;
    MouseEvent me{ MouseEvent::EventType::LPress, x, y };
    eventBuffer.push(me);
}

void Mouse::onLeftReleased(int x, int y) {
    leftDown = false;
    MouseEvent me{ MouseEvent::EventType::LRelease, x, y };
    eventBuffer.push(me);
}

void Mouse::onRightPressed(int x, int y) {
    rightDown = true;
    MouseEvent me{ MouseEvent::EventType::RPress, x, y };
    eventBuffer.push(me);
}

void Mouse::onRightReleased(int x, int y) {
    rightDown = false;
    MouseEvent me{ MouseEvent::EventType::RRelease, x, y };
    eventBuffer.push(me);
}

void Mouse::onMiddlePressed(int x, int y) {
    middleDown = true;
    MouseEvent me{ MouseEvent::EventType::MPress, x, y };
    eventBuffer.push(me);
}

void Mouse::onMiddleReleased(int x, int y) {
    rightDown = false;
    MouseEvent me{ MouseEvent::EventType::MRelease, x, y };
    eventBuffer.push(me);
}

void Mouse::onWheelUp(int x, int y) {
    eventBuffer.push(MouseEvent{ MouseEvent::EventType::WheelUp, x, y });
}

void Mouse::onWheelDown(int x, int y) {
    eventBuffer.push(MouseEvent{ MouseEvent::EventType::WheelDown, x, y });
}

void Mouse::onMouseMove(int x, int y) {
    this->x = x;
    this->y = y;
    eventBuffer.push(MouseEvent{ MouseEvent::EventType::Move, x, y });
}

void HID::Mouse::onMouseMoveRaw(int x, int y) {
    eventBuffer.push(MouseEvent{ MouseEvent::EventType::RAW_MOVE, x, y });
}

bool Mouse::isLeftDown() {
    return leftDown;
}

bool Mouse::isMiddleDown() {
    return middleDown;
}

bool Mouse::isRightDown() {
    return rightDown;
}

int Mouse::getPosX() {
    return x;
}

int Mouse::getPosY() {
    return y;
}

MousePoint Mouse::getPos() {
    return MousePoint(x, y);
}

bool Mouse::isEventBufferEmpty() {
    return eventBuffer.empty();
}

MouseEvent Mouse::readEvent() {
    if (isEventBufferEmpty()) {
        return MouseEvent();
    }
    else {
        MouseEvent me{ eventBuffer.front() };
        eventBuffer.pop();
        return me;
    }    
}
