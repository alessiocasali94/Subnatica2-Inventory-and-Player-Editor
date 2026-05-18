#pragma once

#include "app/Window.h"
#include "config/IniConfig.h"
#include "functions/FunctionRegistry.h"
#include "gfx/OceanBackground.h"
#include "gfx/Renderer.h"
#include "themes/ThemeManager.h"
#include "ui/TrainerUI.h"
#include "util/Hotkeys.h"
#include "util/KeyPoll.h"
#include "util/LogBuffer.h"

struct ID3D11Device;

namespace app {

class Application {
public:
    explicit Application(HINSTANCE inst);
    int Run();

private:
    bool Init();
    void Shutdown();
    void Frame();
    void SaveIfNeeded();

    HINSTANCE inst_;
    Window window_;
    gfx::Renderer renderer_;
    themes::ThemeManager themes_;
    functions::FunctionRegistry registry_;
    util::LogBuffer log_;
    util::Hotkeys hotkeys_;
    util::KeyPoll keys_;
    config::IniConfig ini_;
    config::AppSettings settings_;
    ui::TrainerUI ui_;

    bool running_ = true;
    bool menuVisible_ = true;
    float lastFrame_ = 0.f;
};

} // namespace app
