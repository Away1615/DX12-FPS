#pragma once
#define NOMINMAX
#define WINDOW_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define WINDOW_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#include <Windows.h>
#include <string>

class Window;

extern Window* window;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Window {
public:
    HWND hwnd = nullptr;

    bool keys[256] = {};
    bool mouseButtons[3] = {};

    int mouseX = 0;
    int mouseY = 0;

    int mouseDeltaX = 0;
    int mouseDeltaY = 0;
    int lastMouseX = 0;
    int lastMouseY = 0;

    int width = 0;
    int height = 0;

public:
    Window() = default;
    ~Window() = default;

    void create(int x, int y, int w, int h, const std::string& title, bool fullWindow);
    void processMessages();
    void handleMouseMove(LPARAM lParam);

private:
    HINSTANCE hinstance = nullptr;

    void resetCursorToCenter();
    void calculateDeltaAndAbsolutePos(
        int x_screen, int y_screen,
        int x_client, int y_client
    );
};
