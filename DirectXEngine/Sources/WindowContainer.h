#pragma once

#include "RenderWindow.h"
#include "Keyboard/KeyboardClass.h"
#include "Mouse/MouseClass.h"
#include "Graphics/Graphics.h"

class WindowContainer {
    using key_type = Keyboard::key_type;

public:
    WindowContainer();
    LRESULT windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
    RenderWindow renderWindow;
    Keyboard::KeyboardClass keyboard;
    Mouse::MouseClass mouse;
    Graphics gfx;
};