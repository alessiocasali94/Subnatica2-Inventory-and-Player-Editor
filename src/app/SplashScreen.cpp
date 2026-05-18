#include "app/SplashScreen.h"

#include "gfx/OceanBackground.h"
#include "gfx/Renderer.h"
#include "themes/ThemeManager.h"
#include "util/Time.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <dwmapi.h>
#include <algorithm>
#include <cmath>

#pragma comment(lib, "dwmapi.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
                                                             LPARAM lParam);

namespace app {

namespace {
SplashScreen* g_splash = nullptr;

void EnableDwmFrame(HWND hwnd) {
    MARGINS margins = {0, 0, 0, 0};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
}

void ApplyRoundCorners(HWND hwnd) {
    const int pref = 2;
    DwmSetWindowAttribute(hwnd, 33, &pref, sizeof(pref));
}
} // namespace

SplashScreen::SplashScreen(HINSTANCE inst) : inst_(inst) {}

SplashScreen::~SplashScreen() {
    Close();
}

LRESULT CALLBACK SplashScreen::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp))
        return true;
    return DefWindowProcW(hwnd, msg, wp, lp);
}

bool SplashScreen::CreateWindowCentered() {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst_;
    wc.hIcon = LoadIconW(inst_, MAKEINTRESOURCEW(1));
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"SabNavrSplashWnd";
    RegisterClassExW(&wc);

    const int sx = GetSystemMetrics(SM_CXSCREEN);
    const int sy = GetSystemMetrics(SM_CYSCREEN);
    const int x = (sx - width_) / 2;
    const int y = (sy - height_) / 2;

    hwnd_ = CreateWindowExW(WS_EX_APPWINDOW, wc.lpszClassName, L"Subnautica 2",
                            WS_POPUP, x, y, width_, height_, nullptr, nullptr, inst_, nullptr);
    if (!hwnd_)
        return false;

    g_splash = this;
    EnableDwmFrame(hwnd_);
    ApplyRoundCorners(hwnd_);
    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);
    return true;
}

void SplashScreen::Close() {
    if (hwnd_) {
        ::DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
    g_splash = nullptr;
}

void SplashScreen::PumpMessages() {
    MSG msg{};
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void SplashScreen::RenderFrame(float globalTime, float alpha, float progress) {
    themes::ThemeManager themes;
    themes.Set(themes::Preset::AbyssDeep);
    themes.ApplyImGuiStyle(ImGui::GetStyle());

    const auto& theme = themes.Active();
    gfx::OceanBackground ocean;
    ocean.Update(0.016f, theme, 420);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(width_), static_cast<float>(height_)));
    ImGui::Begin("##Splash", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

    ImDrawList* bg = ImGui::GetWindowDrawList();
    ImVec2 o = ImGui::GetWindowPos();
    ImVec2 sz = ImGui::GetWindowSize();
    ocean.Draw(bg, o, sz, theme, globalTime);

    ImU32 glass = ImGui::ColorConvertFloat4ToU32(
        ImVec4(theme.panel.x, theme.panel.y, theme.panel.z, theme.panel.w * 0.88f));
    ImU32 border = ImGui::ColorConvertFloat4ToU32(theme.panelBorder);
    const float pad = 22.f;
    ImVec2 inner(o.x + pad, o.y + pad);
    ImVec2 inner2(o.x + sz.x - pad, o.y + sz.y - pad);
    bg->AddRectFilled(inner, inner2, glass, 14.f);
    bg->AddRect(inner, inner2, border, 14.f, 0, 1.6f);

    const ImVec2 center(o.x + sz.x * 0.5f, o.y + sz.y * 0.48f);
    const char* title = "SUBNAUTICA 2";
    const ImVec2 ts = ImGui::CalcTextSize(title);
    const ImVec2 tp(center.x - ts.x * 0.5f, center.y - ts.y * 0.5f - 8.f);
    ImU32 glow = ImGui::ColorConvertFloat4ToU32(
        ImVec4(theme.glow.x, theme.glow.y, theme.glow.z, theme.glow.w * 1.1f));
    for (int i = 0; i < 4; ++i)
        bg->AddText(ImVec2(tp.x - static_cast<float>(i), tp.y), glow, title);
    bg->AddText(tp, ImGui::ColorConvertFloat4ToU32(theme.text), title);

    const char* sub = "Loading";
    const ImVec2 ss = ImGui::CalcTextSize(sub);
    bg->AddText(ImVec2(center.x - ss.x * 0.5f, tp.y + ts.y + 10.f),
                ImGui::ColorConvertFloat4ToU32(theme.textDim), sub);

    const float t = std::clamp(progress, 0.f, 1.f);
    const float trackW = inner2.x - inner.x - 40.f;
    const float barW = trackW * t;
    const float barX = inner.x + 20.f;
    bg->AddRectFilled(ImVec2(barX, inner2.y - 30.f), ImVec2(barX + trackW, inner2.y - 24.f),
                      ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 0.35f)), 2.f);
    const float barY = inner2.y - 28.f;
    bg->AddRectFilled(ImVec2(barX, barY), ImVec2(barX + barW, barY + 3.f),
                      ImGui::ColorConvertFloat4ToU32(theme.accent), 2.f);

    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::Render();
}

bool SplashScreen::Run(float durationSeconds) {
    if (!CreateWindowCentered())
        return false;

    gfx::Renderer renderer;
    if (!renderer.Init(hwnd_, width_, height_))
        return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    ImGui_ImplWin32_Init(hwnd_);
    ImGui_ImplDX11_Init(renderer.Device(), renderer.Context());
    io.Fonts->AddFontDefault();
    io.FontGlobalScale = 1.15f;

    const double start = util::NowSeconds();
    const double end = start + static_cast<double>(durationSeconds);

    while (util::NowSeconds() < end) {
        PumpMessages();

        const double elapsed = util::NowSeconds() - start;
        float alpha = 1.f;
        if (elapsed < 0.35)
            alpha = static_cast<float>(elapsed / 0.35);
        const double remain = end - util::NowSeconds();
        if (remain < 0.4)
            alpha *= static_cast<float>(remain / 0.4);

        const float progress = static_cast<float>(elapsed / durationSeconds);
        RenderFrame(static_cast<float>(elapsed), alpha, progress);

        const float clear[4] = {0.02f, 0.05f, 0.12f, 1.f};
        renderer.BeginFrame(clear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        renderer.EndFrame();

        Sleep(16);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    renderer.Shutdown();
    Close();
    return true;
}

} // namespace app
