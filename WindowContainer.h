#pragma once

#include "RenderWindow.h"
#include "HID/Keyboard/KeyboardClass.h"
#include "HID/Mouse/MouseClass.h"
#include "Graphics/Graphics.h"

class WindowContainer {
    using key_type = NKeyboard::key_type;

public:
    WindowContainer();
    LRESULT windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
    RenderWindow renderWindow;
    NKeyboard::Keyboard keyboard;
    NMouse::Mouse mouse;
    NGraphics::Graphics gfx;
};