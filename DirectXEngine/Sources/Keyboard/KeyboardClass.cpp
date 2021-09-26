#pragma once

#include "KeyboardClass.h"

using namespace Keyboard;

KeyboardClass::KeyboardClass() {
    for (int i = 0; i < keyStatesSize; i++) {
        keyStates[i] = false;
    }
}

bool KeyboardClass::isKeyPressed(const key_type keycode) {
    return keyStates[keycode];
}

bool KeyboardClass::isKeyBufferEmpty() {
    return keyBuffer.empty();
}

bool KeyboardClass::isCharBufferEmpty() {
    return charBuffer.empty();
}

KeyboardEvent KeyboardClass::readKey() {
    if (isKeyBufferEmpty()) {
        return KeyboardEvent();
    }
    KeyboardEvent e = keyBuffer.front();
    keyBuffer.pop();
    return e;
}

unsigned char KeyboardClass::readChar() {
    if (isCharBufferEmpty()) {
        return 0u;
    }
    key_type chr = charBuffer.front();
    charBuffer.pop();
    return chr;
}

void KeyboardClass::onKeyPressed(const key_type key) {
    keyStates[key] = true;
    keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void KeyboardClass::onKeyReleased(const key_type key) {
    keyStates[key] = false;
    keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void KeyboardClass::onChar(const key_type key) {
    charBuffer.push(key);
}

void KeyboardClass::enableAutoRepeatKeys() {
    autoRepeatKeys = true;
}

void KeyboardClass::DisableAutoRepeatKeys() {
    autoRepeatKeys = false;
}

void KeyboardClass::enableAutoRepeatChars() {
    autoRepeatChars = true;
}

void KeyboardClass::DisableAutoRepeatChars() {
    autoRepeatChars = false;
}

bool KeyboardClass::isKeysAutoRepeat() {
    return autoRepeatKeys;
}

bool KeyboardClass::isCharsAutoRepeat() {
    return autoRepeatChars;
}
