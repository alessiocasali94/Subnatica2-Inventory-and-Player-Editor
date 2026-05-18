#pragma once

#include "config/IniConfig.h"
#include "functions/FunctionRegistry.h"
#include "gfx/OceanBackground.h"
#include "themes/ThemeManager.h"
#include "util/LogBuffer.h"

namespace ui {

class TrainerUI {
public:
    TrainerUI(functions::FunctionRegistry& reg, themes::ThemeManager& themes, util::LogBuffer& log);

    void Draw(config::AppSettings& app, float dt, float globalTime, bool& menuVisible);
    functions::Category SelectedCategory() const { return selected_; }

    void ApplyPreset(const std::string& name);
    void ToggleFunctionById(const std::string& id);

private:
    void DrawHeader(config::AppSettings& app);
    void DrawSidebar();
    void DrawSidebarFooter();
    void DrawContent(config::AppSettings& app);
    void DrawFunction(functions::TrainerFunction& fn, config::AppSettings& app);
    void DrawMinimap();
    void DrawLogPanel();
    void DrawHotkeyPanel();
    void DrawPresetsTab(config::AppSettings& app);
    void DrawFakeEchoMap();

    functions::FunctionRegistry& reg_;
    themes::ThemeManager& themes_;
    util::LogBuffer& log_;
    gfx::OceanBackground ocean_;

    functions::Category selected_ = functions::Category::PlayerStats;
    float contentAlpha_ = 1.f;
    float uiFade_ = 1.f;
    double openTime_ = 0.0;
};

} // namespace ui
