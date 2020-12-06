#pragma once

#include "KeyboardClass.h"

using namespace NKeyboard;

Keyboard::Keyboard() {
    for (int i = 0; i < keyStatesSize; i++) {
        keyStates[i] = false;
    }
}

bool Keyboard::isKeyPressed(const key_type keycode) {
    return keyStates[keycode];
}

bool Keyboard::isKeyBufferEmpty() {
    return keyBuffer.empty();
}

bool Keyboard::isCharBufferEmpty() {
    return charBuffer.empty();
}

KeyboardEvent Keyboard::readKey() {
    if (isKeyBufferEmpty()) {
        return KeyboardEvent();
    }
    KeyboardEvent e = keyBuffer.front();
    keyBuffer.pop();
    return e;
}

unsigned char Keyboard::readChar() {
    if (isCharBufferEmpty()) {
        return 0u;
    }
    key_type chr = charBuffer.front();
    charBuffer.pop();
    return chr;
}

void Keyboard::onKeyPressed(const key_type key) {
    keyStates[key] = true;
    keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void Keyboard::onKeyReleased(const key_type key) {
    keyStates[key] = false;
    keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void Keyboard::onChar(const key_type key) {
    charBuffer.push(key);
}

void Keyboard::enableAutoRepeatKeys() {
    autoRepeatKeys = true;
}

void Keyboard::DisableAutoRepeatKeys() {
    autoRepeatKeys = false;
}

void Keyboard::enableAutoRepeatChars() {
    autoRepeatChars = true;
}

void Keyboard::DisableAutoRepeatChars() {
    autoRepeatChars = false;
}

bool Keyboard::isKeysAutoRepeat() {
    return autoRepeatKeys;
}

bool Keyboard::isCharsAutoRepeat() {
    return autoRepeatChars;
}
