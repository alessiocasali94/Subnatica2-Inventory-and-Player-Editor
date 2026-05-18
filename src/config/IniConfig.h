#pragma once

#include "functions/FunctionRegistry.h"
#include "themes/ThemeManager.h"

#include <string>

namespace config {

struct AppSettings {
    int windowX = 100;
    int windowY = 80;
    int windowW = 1280;
    int windowH = 800;
    int themeIndex = 0;
    bool overlayMode = false;
    bool soundEnabled = true;
    bool menuVisible = true;
    bool saveRequested = false;
    std::string lastPreset;
};

class IniConfig {
public:
    explicit IniConfig(std::string path);

    void Load(AppSettings& app, themes::ThemeManager& themes, functions::FunctionRegistry& reg);
    void Save(const AppSettings& app, const themes::ThemeManager& themes,
              const functions::FunctionRegistry& reg) const;

private:
    std::string path_;
};

} // namespace config
