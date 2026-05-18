#include "themes/ThemeManager.h"

#include "imgui.h"

namespace themes {

OceanTheme ThemeFor(Preset p) {
    OceanTheme t;
    switch (p) {
    case Preset::AbyssDeep:
        t.name = "Abyss Deep";
        t.bgDeep = ImVec4(0.01f, 0.03f, 0.1f, 0.94f);
        t.bgShallow = ImVec4(0.04f, 0.14f, 0.24f, 0.88f);
        t.accent = ImVec4(0.15f, 0.75f, 0.82f, 1.f);
        t.waveSpeed = 0.3f;
        break;
    case Preset::Twilight:
        t.name = "Twilight";
        t.bgDeep = ImVec4(0.06f, 0.04f, 0.14f, 0.93f);
        t.bgShallow = ImVec4(0.2f, 0.1f, 0.35f, 0.85f);
        t.accent = ImVec4(0.65f, 0.45f, 0.95f, 1.f);
        t.glow = ImVec4(0.5f, 0.3f, 0.9f, 0.3f);
        t.waveSpeed = 0.45f;
        break;
    case Preset::Bioluminescent:
        t.name = "Bioluminescent";
        t.bgDeep = ImVec4(0.01f, 0.08f, 0.1f, 0.92f);
        t.bgShallow = ImVec4(0.05f, 0.28f, 0.22f, 0.86f);
        t.accent = ImVec4(0.2f, 1.f, 0.55f, 1.f);
        t.glow = ImVec4(0.1f, 1.f, 0.6f, 0.45f);
        t.particleDensity = 1.4f;
        t.waveSpeed = 0.55f;
        break;
    case Preset::Classic:
        t.name = "Classic";
        t.bgDeep = ImVec4(0.05f, 0.08f, 0.12f, 0.9f);
        t.bgShallow = ImVec4(0.12f, 0.22f, 0.32f, 0.82f);
        t.accent = ImVec4(0.35f, 0.7f, 0.95f, 1.f);
        t.text = ImVec4(0.9f, 0.93f, 0.95f, 0.92f);
        t.particleDensity = 0.7f;
        break;
    default:
        break;
    }
    return t;
}

const char* PresetName(Preset p) {
    return ThemeFor(p).name;
}

ThemeManager::ThemeManager() {
    Set(Preset::AbyssDeep);
}

void ThemeManager::Set(Preset p) {
    current_ = p;
    active_ = ThemeFor(p);
}

void ThemeManager::Next() {
    int n = (static_cast<int>(current_) + 1) % static_cast<int>(Preset::Count);
    Set(static_cast<Preset>(n));
}

void ThemeManager::SetPresetIndex(int i) {
    if (i < 0 || i >= static_cast<int>(Preset::Count))
        return;
    Set(static_cast<Preset>(i));
}

void ThemeManager::ApplyImGuiStyle(ImGuiStyle& s) const {
    const auto& t = active_;
    s.WindowRounding = 14.f;
    s.ChildRounding = 10.f;
    s.FrameRounding = 8.f;
    s.GrabRounding = 6.f;
    s.ScrollbarRounding = 8.f;
    s.WindowBorderSize = 1.f;
    s.FrameBorderSize = 1.f;
    s.GrabMinSize = 14.f;
    s.WindowPadding = ImVec2(14, 12);
    s.ItemSpacing = ImVec2(10, 8);
    s.ScrollbarSize = 10.f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_Text] = t.text;
    c[ImGuiCol_TextDisabled] = t.textDim;
    c[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_ChildBg] = t.panel;
    c[ImGuiCol_PopupBg] = ImVec4(t.panel.x, t.panel.y, t.panel.z, 0.96f);
    c[ImGuiCol_Border] = t.panelBorder;
    c[ImGuiCol_FrameBg] = ImVec4(0.04f, 0.12f, 0.16f, 0.92f);
    c[ImGuiCol_FrameBgHovered] = ImVec4(t.accentDim.x, t.accentDim.y, t.accentDim.z, 0.55f);
    c[ImGuiCol_FrameBgActive] = ImVec4(t.accent.x, t.accent.y, t.accent.z, 0.4f);
    c[ImGuiCol_TitleBg] = t.bgDeep;
    c[ImGuiCol_TitleBgActive] = t.bgShallow;
    c[ImGuiCol_Header] = ImVec4(t.accent.x, t.accent.y, t.accent.z, 0.2f);
    c[ImGuiCol_HeaderHovered] = ImVec4(t.accent.x, t.accent.y, t.accent.z, 0.35f);
    c[ImGuiCol_HeaderActive] = ImVec4(t.accent.x, t.accent.y, t.accent.z, 0.5f);
    c[ImGuiCol_Button] = ImVec4(t.accentDim.x, t.accentDim.y, t.accentDim.z, 0.4f);
    c[ImGuiCol_ButtonHovered] = ImVec4(t.accent.x, t.accent.y, t.accent.z, 0.45f);
    c[ImGuiCol_ButtonActive] = ImVec4(t.accent.x, t.accent.y, t.accent.z, 0.65f);
    c[ImGuiCol_CheckMark] = t.accent;
    c[ImGuiCol_SliderGrab] = t.accent;
    c[ImGuiCol_SliderGrabActive] = ImVec4(t.accent.x * 1.1f, t.accent.y * 1.1f, t.accent.z, 1.f);
    c[ImGuiCol_ScrollbarBg] = ImVec4(0, 0, 0, 0.1f);
    c[ImGuiCol_ScrollbarGrab] = ImVec4(t.accentDim.x, t.accentDim.y, t.accentDim.z, 0.5f);
    c[ImGuiCol_ScrollbarGrabHovered] = t.accent;
    c[ImGuiCol_ScrollbarGrabActive] = t.accent;
    c[ImGuiCol_Separator] = t.panelBorder;
    c[ImGuiCol_Tab] = ImVec4(t.panel.x, t.panel.y, t.panel.z, 0.8f);
    c[ImGuiCol_TabHovered] = ImVec4(t.accent.x, t.accent.y, t.accent.z, 0.4f);
    c[ImGuiCol_TabActive] = ImVec4(t.accent.x, t.accent.y, t.accent.z, 0.55f);
}

} // namespace themes
