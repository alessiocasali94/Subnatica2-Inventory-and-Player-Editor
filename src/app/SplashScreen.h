#pragma once

#include <windows.h>

namespace app {

class SplashScreen {
public:
    explicit SplashScreen(HINSTANCE inst);
    ~SplashScreen();

    bool Run(float durationSeconds);

private:
    bool CreateWindowCentered();
    void Close();
    void PumpMessages();
    void RenderFrame(float globalTime, float alpha, float progress);

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

    HINSTANCE inst_ = nullptr;
    HWND hwnd_ = nullptr;
    int width_ = 440;
    int height_ = 260;
};

} // namespace app
