#pragma once

#include "WindowContainer.h"

LRESULT WindowContainer::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CHAR: {
            auto ch = static_cast<key_type>(wParam);
            // 30-th bit tells whether the button was pressed already, see:
            // https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-char
            const bool wasPressed = lParam & (1 << 30);
            if (keyboard.isCharsAutoRepeat() || !wasPressed) {
                keyboard.onChar(ch);
            }
            return 0;
        }
        case WM_KEYDOWN: {
            auto keycode = static_cast<key_type>(wParam);
            const bool wasPressed = lParam & (1 << 30);
            if (keyboard.isKeysAutoRepeat() || !wasPressed) {
                keyboard.onKeyPressed(keycode);
            }
            return 0;
        }
        case WM_KEYUP: {
            auto keycode = static_cast<key_type>(wParam);
            keyboard.onKeyReleased(keycode);
            return 0;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
}
