#pragma once

#include "RenderWindow.h"
#include "Keyboard/KeyboardClass.h"
#include "Mouse/MouseClass.h"

class WindowContainer {
    using key_type = Keyboard::key_type;

public:
    LRESULT windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
    RenderWindow renderWindow;
    Keyboard::KeyboardClass keyboard;
    Mouse::MouseClass mouse;
};