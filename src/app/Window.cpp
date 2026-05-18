#include "app/Window.h"

#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

namespace app {

namespace {
Window* g_instance = nullptr;

void EnableDwmFrame(HWND hwnd) {
    MARGINS margins = {0, 0, 0, 0};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
}

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

constexpr int kCornerRadius = 18;

void ApplyRoundCorners(HWND hwnd, int width, int height) {
    if (width <= 0 || height <= 0)
        return;

    const int pref = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));

    // DWM rounded corners only — SetWindowRgn can break D3D11 present (black client area).
}
} // namespace

Window* Window::FromHwnd(HWND hwnd) {
    return reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
}

bool Window::Create(HINSTANCE inst, int width, int height) {
    width_ = width;
    height_ = height;
    g_instance = this;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst;
    wc.hIcon = LoadIconW(inst, MAKEINTRESOURCEW(1));
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"SabNavrMainWnd";
    RegisterClassExW(&wc);

    hwnd_ = CreateWindowExW(
        WS_EX_APPWINDOW, wc.lpszClassName, L"Subnautica 2",
        WS_POPUP, 100, 80, width_, height_, nullptr, nullptr, inst, nullptr);

    if (!hwnd_)
        return false;

    SetWindowLongPtrW(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    EnableDwmFrame(hwnd_);
    ApplyRoundCorners(hwnd_, width_, height_);
    ApplyOverlayMode();
    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);

    return true;
}

void Window::Destroy() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
    g_instance = nullptr;
}

void Window::PumpMessages() {
    MSG msg{};
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void Window::SetVisible(bool visible) {
    visible_ = visible;
    if (hwnd_)
        ShowWindow(hwnd_, visible ? SW_SHOW : SW_HIDE);
}

bool Window::ConsumeMenuToggle() {
    if (!toggleRequested_)
        return false;
    toggleRequested_ = false;
    return true;
}

bool Window::ConsumeMenuHide() {
    if (!hideRequested_)
        return false;
    hideRequested_ = false;
    return true;
}

void Window::SetOverlayMode(bool overlay) {
    if (overlay_ == overlay)
        return;
    overlay_ = overlay;
    ApplyOverlayMode();
}

void Window::ApplyOverlayMode() {
    if (!hwnd_)
        return;
    SetWindowPos(hwnd_, overlay_ ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Window* self = FromHwnd(hwnd);
    if (self && self->wndProcChain_ && self->wndProcChain_(hwnd, msg, wp, lp))
        return true;

    if (self && self->messageHook_)
        self->messageHook_(msg, wp, lp);

    switch (msg) {
    case WM_KEYDOWN:
        if (self && !(lp & 0x40000000)) {
            if (wp == VK_INSERT)
                self->toggleRequested_ = true;
            else if (wp == VK_HOME)
                self->hideRequested_ = true;
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wp & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_CLOSE:
        if (self)
            self->closed_ = true;
        DestroyWindow(hwnd);
        return 0;
    case WM_SIZE:
        if (self) {
            self->width_ = LOWORD(lp);
            self->height_ = HIWORD(lp);
            ApplyRoundCorners(hwnd, self->width_, self->height_);
            if (self->resizeHook_)
                self->resizeHook_(self->width_, self->height_);
        }
        return 0;
    case WM_DESTROY:
        if (self)
            self->closed_ = true;
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

} // namespace app
