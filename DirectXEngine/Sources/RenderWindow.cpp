#include "WindowContainer.h"
#include "Utils/StringHelper.h"

bool RenderWindow::initialize(
    WindowContainer* pWindowContainer,
    HINSTANCE hInstance,
    std::string windowTitle,
    std::string windowClass,
    int width,
    int height
) {
    this->hInstance = hInstance;
    this->windowTitle = windowTitle;
    windowTitleWide = StringHelper::stringToWide(windowTitle).c_str();
    this->windowClass = windowClass;
    windowClassWide = StringHelper::stringToWide(windowClass).c_str();
    this->width = width;
    this->height = height;

    registerWindowClass();

    int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
    int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

    RECT wr; // Window rectangle
    wr.left = centerScreenX;
    wr.top = centerScreenY;
    wr.right = wr.left + width;
    wr.bottom = wr.top + height; 

    AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
    
    handle = CreateWindowEx(0,
        windowClassWide.c_str(),
        windowTitleWide.c_str(),
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        wr.left, wr.top, 
        wr.right - wr.left, wr.bottom - wr.top,
        nullptr,
        nullptr,
        this->hInstance,
        pWindowContainer);

    if (handle == nullptr) {
        ErrorLogger::log(GetLastError(), "CreateWindowEx Failed for window: " + this->windowTitle);
        return false;
    }

    ShowWindow(handle, SW_SHOW);
    SetForegroundWindow(handle);
    SetFocus(handle);
    return true;
}

bool RenderWindow::processMessages() {
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    while (PeekMessage(&msg, handle, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (msg.message == WM_NULL) {
        if (!IsWindow(handle)) {
            handle = nullptr;
            UnregisterClass(windowClassWide.c_str(), hInstance);
            return false;
        }
    }

    return true;
}

HWND RenderWindow::getHWND() const {
    return handle;
}

RenderWindow::~RenderWindow() {
    if (handle != nullptr) {
        UnregisterClass(windowClassWide.c_str(), hInstance);
        DestroyWindow(handle);
    }
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    default:
        WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        return pWindow->windowProc(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_NCCREATE: {
            const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
            WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
            if (pWindow == nullptr) {
                ErrorLogger::log("Critical Error: Pointer to window container is null during WM_NCCREATE.");
                exit(1);
            }
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
            return pWindow->windowProc(hwnd, uMsg, wParam, lParam);
        }
        default: {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
}

void RenderWindow::registerWindowClass() {
    WNDCLASSEX wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    // TODO: check whether its always call setup
    wc.lpfnWndProc = HandleMessageSetup;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = nullptr;
    wc.hIconSm = nullptr;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = windowClassWide.c_str();
    wc.cbSize = sizeof(WNDCLASSEX);
    RegisterClassEx(&wc);
}
