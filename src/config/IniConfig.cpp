#include "config/IniConfig.h"

#include <fstream>
#include <sstream>

namespace config {

namespace {

std::string NormalizeFunctionId(std::string id) {
    if (id == "god_mode")
        return "player_invuln";
    if (id == "noclip")
        return "free_move";
    if (id == "fly_mode")
        return "aerial_move";
    if (id == "vehicle_god")
        return "vehicle_invuln";
    return id;
}

std::string Trim(std::string s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' '))
        s.pop_back();
    return s;
}
} // namespace

IniConfig::IniConfig(std::string path) : path_(std::move(path)) {}

void IniConfig::Load(AppSettings& app, themes::ThemeManager& themes,
                     functions::FunctionRegistry& reg) {
    std::ifstream in(path_);
    if (!in)
        return;
    std::string line, section;
    while (std::getline(in, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;
        if (line.front() == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
            continue;
        }
        auto eq = line.find('=');
        if (eq == std::string::npos)
            continue;
        std::string key = Trim(line.substr(0, eq));
        std::string val = Trim(line.substr(eq + 1));

        if (section == "Window") {
            if (key == "X")
                app.windowX = std::stoi(val);
            else if (key == "Y")
                app.windowY = std::stoi(val);
            else if (key == "W")
                app.windowW = std::stoi(val);
            else if (key == "H")
                app.windowH = std::stoi(val);
            else if (key == "Overlay")
                app.overlayMode = (val == "1");
            else if (key == "Visible")
                app.menuVisible = (val != "0");
        } else if (section == "App") {
            if (key == "Theme")
                themes.SetPresetIndex(std::stoi(val));
            else if (key == "Sound")
                app.soundEnabled = (val == "1");
            else if (key == "LastPreset")
                app.lastPreset = val;
        } else if (section == "Functions") {
            key = NormalizeFunctionId(std::move(key));
            if (auto* fn = reg.Find(key)) {
                if (val.find(',') != std::string::npos) {
                    auto comma = val.find(',');
                    fn->enabled = (val.substr(0, comma) == "1");
                    fn->value = std::stof(val.substr(comma + 1));
                } else {
                    fn->enabled = (val == "1");
                }
            }
        }
    }
}

void IniConfig::Save(const AppSettings& app, const themes::ThemeManager& themes,
                     const functions::FunctionRegistry& reg) const {
    std::ofstream out(path_);
    if (!out)
        return;
    out << "; Subnautica 2 — visual settings\n";
    out << "[Window]\n";
    out << "X=" << app.windowX << "\n";
    out << "Y=" << app.windowY << "\n";
    out << "W=" << app.windowW << "\n";
    out << "H=" << app.windowH << "\n";
    out << "Overlay=" << (app.overlayMode ? 1 : 0) << "\n";
    out << "Visible=" << (app.menuVisible ? 1 : 0) << "\n";
    out << "[App]\n";
    out << "Theme=" << themes.PresetIndex() << "\n";
    out << "Sound=" << (app.soundEnabled ? 1 : 0) << "\n";
    out << "LastPreset=" << app.lastPreset << "\n";
    out << "[Functions]\n";
    for (const auto& fn : reg.All()) {
        out << fn.id << "=" << (fn.enabled ? 1 : 0);
        if (fn.HasSlider() || fn.kind == functions::FunctionKind::Slider)
            out << "," << fn.value;
        out << "\n";
    }
}

} // namespace config
