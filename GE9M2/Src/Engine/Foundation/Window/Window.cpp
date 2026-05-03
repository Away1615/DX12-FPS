#include "Window.h"

Window* window = nullptr;

void Window::resetCursorToCenter() {
    ShowCursor(FALSE);

    POINT centerClient = { width / 2, height / 2 };
    POINT centerScreen = centerClient;
    ClientToScreen(hwnd, &centerScreen);
    SetCursorPos(centerScreen.x, centerScreen.y);

    lastMouseX = centerScreen.x;
    lastMouseY = centerScreen.y;
}

void Window::calculateDeltaAndAbsolutePos(
    int x_screen, int y_screen,
    int x_client, int y_client
) {
    mouseDeltaX = x_screen - lastMouseX;
    mouseDeltaY = y_screen - lastMouseY;

    mouseX = x_client;
    mouseY = y_client;
}

void Window::create(
    int x, int y, int w, int h,
    const std::string& title,
    bool fullWindow
) {
    width = w;
    height = h;
    hinstance = GetModuleHandle(nullptr);

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hinstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    std::wstring wname(title.begin(), title.end());
    wc.lpszClassName = wname.c_str();
    RegisterClassEx(&wc);

    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    int finalX = x;
    int finalY = y;

    if (fullWindow) {
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
        style = WS_POPUP | WS_VISIBLE;
        finalX = 0;
        finalY = 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        wname.c_str(),
        wname.c_str(),
        style,
        finalX, finalY,
        width, height,
        nullptr, nullptr,
        hinstance,
        this
    );

    resetCursorToCenter();
    mouseDeltaX = 0;
    mouseDeltaY = 0;

    window = this;

    if (fullWindow) {
        SetForegroundWindow(hwnd);
    }
}

void Window::processMessages() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Window::handleMouseMove(LPARAM lParam) {
    int x_client = WINDOW_GET_X_LPARAM(lParam);
    int y_client = WINDOW_GET_Y_LPARAM(lParam);

    POINT p = { x_client, y_client };
    ClientToScreen(hwnd, &p);

    calculateDeltaAndAbsolutePos(p.x, p.y, x_client, y_client);
    resetCursorToCenter();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        if (wParam < 256 && window)
            window->keys[wParam] = true;
        return 0;

    case WM_KEYUP:
        if (wParam < 256 && window)
            window->keys[wParam] = false;
        return 0;

    case WM_LBUTTONDOWN:
        if (window) {
            window->mouseX = WINDOW_GET_X_LPARAM(lParam);
            window->mouseY = WINDOW_GET_Y_LPARAM(lParam);
            window->mouseButtons[0] = true;
        }
        return 0;

    case WM_LBUTTONUP:
        if (window) {
            window->mouseX = WINDOW_GET_X_LPARAM(lParam);
            window->mouseY = WINDOW_GET_Y_LPARAM(lParam);
            window->mouseButtons[0] = false;
        }
        return 0;

    case WM_MOUSEMOVE:
        if (window)
            window->handleMouseMove(lParam);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
