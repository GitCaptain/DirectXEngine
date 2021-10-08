#pragma once

#include "RenderWindow.h"
#include "HID/Keyboard.h"
#include "HID/Mouse.h"
#include "Graphics/Graphics.h"

class WindowContainer {
    using key_type = HID::key_type;

public:
    WindowContainer();
    LRESULT windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
    RenderWindow renderWindow;
    HID::Keyboard keyboard;
    HID::Mouse mouse;
    Graphics gfx;
};