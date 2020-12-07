#pragma once
#include <Windows.h>
#include "ErrorLogger.h"

class WindowContainer;

class RenderWindow {

public:
    bool initialize(WindowContainer *pWindowContainer, HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height);
    bool processMessages();
    void onWindowResize(UINT width, UINT height);
    HWND getHWND() const;
    ~RenderWindow();

private:
    void registerWindowClass();
    HWND handle = nullptr;
    HINSTANCE hInstance = nullptr;
    std::string windowTitle = "";
    std::wstring windowTitleWide = L"";
    std::string windowClass = "";
    std::wstring windowClassWide = L"";
    int width = 0;
    int height = 0;
};