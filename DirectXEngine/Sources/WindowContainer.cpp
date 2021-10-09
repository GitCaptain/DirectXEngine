#pragma once

#include "WindowContainer.h"
#include <memory>

WindowContainer::WindowContainer() {
    static bool raw_input_initialized = false;
    if (!raw_input_initialized) {
        RAWINPUTDEVICE rid;
        rid.usUsagePage = 0x01; // mouse
        rid.usUsage = 0x02;
        rid.dwFlags = 0;
        rid.hwndTarget = nullptr;

        if(RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE) {
            ErrorLogger::log(GetLastError(), "Failed to register raw input devices.");
            exit(-1);
        }
        raw_input_initialized = true;
    }
}

#ifdef ENABLE_IMGUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
LRESULT WindowContainer::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

#ifdef ENABLE_IMGUI
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
        return true;
    }
#endif

    switch (uMsg) {
        // keyboard messages
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
        // mouse messages
        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            mouse.onMouseMove(x, y);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            mouse.onLeftPressed(x, y);
            return 0;
        }
        case WM_RBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            mouse.onRightPressed(x, y);
            return 0;
        }
        case WM_MBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            mouse.onMiddlePressed(x, y);
            return 0;
        }
        case WM_LBUTTONUP: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            mouse.onLeftReleased(x, y);
            return 0;
        }
        case WM_RBUTTONUP: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            mouse.onRightReleased(x, y);
            return 0;
        }
        case WM_MBUTTONUP: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            mouse.onMiddleReleased(x, y);
            return 0;
        }
        case WM_MOUSEWHEEL: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (delta > 0) {
                mouse.onWheelUp(x, y);
            }
            else if (delta < 0) {
                mouse.onWheelDown(x, y);
            }
            return 0;
        }
        case WM_INPUT: {
            UINT dataSize=0;
            GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), 
                            RID_INPUT, 
                            nullptr, 
                            &dataSize, 
                            sizeof(RAWINPUTHEADER));
            if (dataSize > 0) {
                std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
                if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
                                    RID_INPUT,
                                    rawdata.get(),
                                    &dataSize,
                                    sizeof(RAWINPUTHEADER)) == dataSize) {
                    auto raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
                    if (raw->header.dwType == RIM_TYPEMOUSE) {
                        mouse.onMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY); 
                    }
                }
            }
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
}
