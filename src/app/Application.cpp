#include "app/Application.h"
#include "app/SplashScreen.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "util/Time.h"

#include <Shlobj.h>
#include <algorithm>
#include <filesystem>
#include <string>
#include <windows.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
                                                             LPARAM lParam);

namespace app {

namespace {

void DrainStaleMessages() {
    MSG msg{};
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message != WM_QUIT) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
}

std::string WideToUtf8(const std::wstring& w) {
    if (w.empty())
        return {};
    const int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()), nullptr, 0,
                                      nullptr, nullptr);
    std::string out(static_cast<size_t>(n), '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()), out.data(), n, nullptr,
                        nullptr);
    return out;
}

std::wstring ConfigPath() {
    wchar_t path[MAX_PATH]{};
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, path))) {
        std::filesystem::path p(path);
        p /= "SabNavrSubnautica2";
        std::filesystem::create_directories(p);
        return (p / "settings.ini").wstring();
    }
    return L"settings.ini";
}
} // namespace

Application::Application(HINSTANCE inst)
    : inst_(inst),
      ini_(WideToUtf8(ConfigPath())),
      ui_(registry_, themes_, log_) {}

bool Application::Init() {
    ini_.Load(settings_, themes_, registry_);
    window_.SetOverlayMode(settings_.overlayMode);
    menuVisible_ = settings_.menuVisible;

    if (!window_.Create(inst_, settings_.windowW, settings_.windowH))
        return false;

    SetWindowPos(window_.Handle(), nullptr, settings_.windowX, settings_.windowY, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER);

    window_.SetVisible(menuVisible_);

    if (!renderer_.Init(window_.Handle(), settings_.windowW, settings_.windowH))
        return false;

    window_.SetResizeHook([this](int w, int h) {
        if (w > 0 && h > 0)
            renderer_.Resize(w, h);
    });

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;

    themes_.ApplyImGuiStyle(ImGui::GetStyle());
    ImGui_ImplWin32_Init(window_.Handle());
    ImGui_ImplDX11_Init(renderer_.Device(), renderer_.Context());

    window_.SetWndProcChain([](HWND h, UINT m, WPARAM w, LPARAM l) {
        return ImGui_ImplWin32_WndProcHandler(h, m, w, l) != 0;
    });

    io.Fonts->AddFontDefault();
    io.Fonts->Build();
    io.FontGlobalScale = 1.05f;
    ImGui_ImplDX11_CreateDeviceObjects();

    hotkeys_.RegisterDefaults(window_.Handle());
    hotkeys_.SetCallback([this](const std::string& id) { ui_.ToggleFunctionById(id); });

    return true;
}

void Application::Shutdown() {
    SaveIfNeeded();
    hotkeys_.Shutdown(window_.Handle());
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    renderer_.Shutdown();
    window_.Destroy();
}

void Application::SaveIfNeeded() {
    RECT r{};
    if (GetWindowRect(window_.Handle(), &r)) {
        settings_.windowX = r.left;
        settings_.windowY = r.top;
        settings_.windowW = r.right - r.left;
        settings_.windowH = r.bottom - r.top;
    }
    settings_.menuVisible = menuVisible_;
    ini_.Save(settings_, themes_, registry_);
}

int Application::Run() {
    SplashScreen splash(inst_);
    if (!splash.Run(4.f))
        return 1;

    DrainStaleMessages();

    if (!Init())
        return 1;

    menuVisible_ = true;
    settings_.menuVisible = true;
    window_.SetVisible(true);

    lastFrame_ = static_cast<float>(util::NowSeconds());

    while (running_ && window_.IsOpen()) {
        window_.PumpMessages();

        // Restore from taskbar-hidden state (only global key poll left).
        if ((!menuVisible_ || !window_.IsVisible()) && keys_.Pressed(VK_INSERT)) {
            menuVisible_ = true;
            window_.SetVisible(true);
            SetForegroundWindow(window_.Handle());
        }
        if (window_.ConsumeMenuToggle()) {
            menuVisible_ = !menuVisible_;
            window_.SetVisible(menuVisible_);
        }
        if (window_.ConsumeMenuHide()) {
            menuVisible_ = false;
            window_.SetVisible(false);
        }

        if (menuVisible_ && window_.IsVisible())
            Frame();
        else
            Sleep(16);
    }

    Shutdown();
    return 0;
}

void Application::Frame() {
    if (!menuVisible_ || !window_.IsVisible())
        return;

    if (settings_.overlayMode != window_.OverlayMode())
        window_.SetOverlayMode(settings_.overlayMode);
    hotkeys_.Poll(window_.Handle());

    float now = static_cast<float>(util::NowSeconds());
    float dt = std::min(now - lastFrame_, 0.05f);
    lastFrame_ = now;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ui_.Draw(settings_, dt, now, menuVisible_);

    if (settings_.saveRequested) {
        settings_.saveRequested = false;
        SaveIfNeeded();
    }

    ImGui::Render();

    const auto& theme = themes_.Active();
    const float clear[4] = {theme.bgDeep.x, theme.bgDeep.y, theme.bgDeep.z, 1.f};
    renderer_.BeginFrame(clear);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    renderer_.EndFrame();

    if (!window_.IsOpen())
        running_ = false;
}

} // namespace app
