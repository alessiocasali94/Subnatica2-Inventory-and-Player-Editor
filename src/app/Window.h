#pragma once

#include <functional>
#include <windows.h>

namespace app {

class Window {
public:
    bool Create(HINSTANCE inst, int width, int height);
    void Destroy();
    void PumpMessages();
    bool IsOpen() const { return hwnd_ != nullptr && !closed_; }
    HWND Handle() const { return hwnd_; }
    int Width() const { return width_; }
    int Height() const { return height_; }

    void SetVisible(bool visible);
    bool IsVisible() const { return visible_; }
    void SetOverlayMode(bool overlay);
    bool OverlayMode() const { return overlay_; }

    using MessageHook = std::function<void(UINT, WPARAM, LPARAM)>;
    void SetMessageHook(MessageHook hook) { messageHook_ = std::move(hook); }

    using WndProcChain = std::function<bool(HWND, UINT, WPARAM, LPARAM)>;
    void SetWndProcChain(WndProcChain chain) { wndProcChain_ = std::move(chain); }

    using ResizeHook = std::function<void(int width, int height)>;
    void SetResizeHook(ResizeHook hook) { resizeHook_ = std::move(hook); }

    bool ConsumeMenuToggle();
    bool ConsumeMenuHide();

    static Window* FromHwnd(HWND hwnd);

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

    HWND hwnd_ = nullptr;
    int width_ = 1280;
    int height_ = 800;
    bool visible_ = true;
    bool overlay_ = false;

    void ApplyOverlayMode();
    bool closed_ = false;
    bool toggleRequested_ = false;
    bool hideRequested_ = false;
    MessageHook messageHook_;
    WndProcChain wndProcChain_;
    ResizeHook resizeHook_;
};

} // namespace app
