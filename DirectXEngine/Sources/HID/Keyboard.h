#pragma once
#include "KeyboardEvent.h"
#include <queue>
#include <limits>
#include <vector>

namespace HID {

    class Keyboard {
    public:
        Keyboard();
        bool isKeyPressed(const key_type keycode);
        bool isKeyBufferEmpty();
        bool isCharBufferEmpty();
        KeyboardEvent readKey();
        unsigned char readChar();
        void onKeyPressed(const key_type  key);
        void onKeyReleased(const key_type  key);
        void onChar(const key_type  key);
        void enableAutoRepeatKeys();
        void DisableAutoRepeatKeys();
        void enableAutoRepeatChars();
        void DisableAutoRepeatChars();
        bool isKeysAutoRepeat();
        bool isCharsAutoRepeat();

    private:
        bool autoRepeatKeys = false;
        bool autoRepeatChars = false;
        const static unsigned int keyStatesSize = std::numeric_limits<key_type>::max() + 1;
        bool keyStates[keyStatesSize];
        std::queue<KeyboardEvent> keyBuffer;
        std::queue<key_type> charBuffer;

    };

}