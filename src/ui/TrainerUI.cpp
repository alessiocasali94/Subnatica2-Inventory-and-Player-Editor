#include "ui/TrainerUI.h"

#include "ui/SmoothScroll.h"
#include "ui/Widgets.h"
#include "ui/WindowChrome.h"
#include "util/Time.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace ui {

TrainerUI::TrainerUI(functions::FunctionRegistry& reg, themes::ThemeManager& themes,
                     util::LogBuffer& log)
    : reg_(reg), themes_(themes), log_(log) {
    openTime_ = util::NowSeconds();
}

void TrainerUI::ToggleFunctionById(const std::string& id) {
    if (auto* fn = reg_.Find(id)) {
        fn->enabled = !fn->enabled;
        log_.AddAtmospheric(fn->label, fn->enabled, reg_.FakeDepthMeters());
    }
}

void TrainerUI::ApplyPreset(const std::string& name) {
    auto apply = [&](const char* id, bool on, float val = 1.f) {
        if (auto* f = reg_.Find(id)) {
            f->enabled = on;
            f->value = val;
        }
    };
    if (name == "Relaxed Explorer") {
        apply("oxygen", true);
        apply("food_water", true);
        apply("walk_speed", false, 1.5f);
        apply("scanner", true, 3.f);
        apply("echo_locator", true);
    } else if (name == "Speedrunner") {
        apply("walk_speed", true, 3.f);
        apply("swim_speed", true, 4.f);
        apply("instant_craft", true);
        apply("game_speed", true, 2.f);
    } else if (name == "Creative Suite") {
        for (auto& f : reg_.All())
            if (f.kind == functions::FunctionKind::Toggle)
                f.enabled = true;
        apply("player_invuln", true);
        apply("free_move", true);
        apply("aerial_move", true, 200.f);
    }
    log_.Add("Preset loaded: " + name);
}

void TrainerUI::Draw(config::AppSettings& app, float dt, float globalTime, bool& menuVisible) {
    const auto& theme = themes_.Active();
    ocean_.Update(dt, theme, reg_.FakeDepthMeters());
    log_.MaybeLeviathanEvent(globalTime);

    uiFade_ += ((menuVisible ? 1.f : 0.f) - uiFade_) * (1.f - std::exp(-8.f * dt));
    contentAlpha_ += (1.f - contentAlpha_) * (1.f - std::exp(-4.f * dt));

    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::SetNextWindowBgAlpha(0.f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, uiFade_);
    if (ImGui::Begin("##Root", nullptr, flags)) {
        ImDrawList* bg = ImGui::GetWindowDrawList();
        ImVec2 o = ImGui::GetWindowPos();
        ImVec2 sz = ImGui::GetWindowSize();
        ocean_.Draw(bg, o, sz, theme, globalTime);

        const float titleH = 34.f;
        ImGui::SetCursorPos(ImVec2(12.f, 8.f));
        DrawWindowTitleBar(theme, sz.x - 24.f, titleH);

        // Glass panel
        ImVec2 pad(40, 30 + titleH);
        ImVec2 panelSize(sz.x - pad.x * 2, sz.y - pad.y * 2);
        ImVec2 panelPos(o.x + pad.x, o.y + pad.y);
        ImU32 glass = ImGui::ColorConvertFloat4ToU32(
            ImVec4(theme.panel.x, theme.panel.y, theme.panel.z, theme.panel.w * 0.92f));
        ImU32 border = ImGui::ColorConvertFloat4ToU32(theme.panelBorder);
        bg->AddRectFilled(panelPos, ImVec2(panelPos.x + panelSize.x, panelPos.y + panelSize.y), glass,
                          16.f);
        bg->AddRect(panelPos, ImVec2(panelPos.x + panelSize.x, panelPos.y + panelSize.y), border,
                    16.f, 0, 1.5f);

        ImGui::SetCursorPos(ImVec2(pad.x + 16, pad.y + 12));
        ImGui::BeginChild("##MainPanel", ImVec2(panelSize.x - 32, panelSize.y - 24), false,
                          ImGuiWindowFlags_NoBackground);

        DrawHeader(app);
        ImGui::Separator();
        ImGui::Spacing();

        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        const float rowAvail = ImGui::GetContentRegionAvail().x;
        float sidebarW = 192.f;
        float rightW = 178.f;
        float contentW = rowAvail - sidebarW - rightW - spacing * 2.f;
        if (contentW < 260.f) {
            const float need = 260.f - contentW;
            rightW = std::max(140.f, rightW - need * 0.75f);
            sidebarW = std::max(168.f, sidebarW - need * 0.25f);
            contentW = rowAvail - sidebarW - rightW - spacing * 2.f;
        }

        ImGui::BeginChild("##Sidebar", ImVec2(sidebarW, -1), true,
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        DrawSidebar();
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("##Content", ImVec2(contentW, -1), true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, contentAlpha_);
        SmoothScrollY("contentScroll");
        DrawContent(app);
        ImGui::PopStyleVar();
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 10.f));
        ImGui::BeginChild("##RightCol", ImVec2(rightW, -1), true);
        DrawMinimap();
        if (reg_.Find("echo_locator") && reg_.Find("echo_locator")->enabled)
            DrawFakeEchoMap();
        DrawHotkeyPanel();
        ImGui::EndChild();
        ImGui::PopStyleVar();

        DrawLogPanel();
        ImGui::EndChild();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void TrainerUI::DrawHeader(config::AppSettings& app) {
    const auto& t = themes_.Active();
    GlowText("SUBNAUTICA 2", t, 1.2f);
    ImGui::SameLine(ImGui::GetWindowWidth() - 420);
    ImGui::TextColored(t.textDim, "v1.2.8 | Depth %dm", reg_.FakeDepthMeters());

    ImGui::Spacing();
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float pulse = 0.5f + 0.5f * std::sin(static_cast<float>(util::NowSeconds()) * 2.f);
    ImU32 dot = ImGui::ColorConvertFloat4ToU32(
        ImVec4(t.accent.x, t.accent.y, t.accent.z, 0.5f + pulse * 0.5f));
    dl->AddCircleFilled(ImVec2(p.x + 6, p.y + 8), 5.f, dot);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
    ImGui::TextColored(t.accent, "Session: Subnautica 2");

    ImGui::SameLine(ImGui::GetWindowWidth() - 200);
    if (RippleButton("Save INI", ImVec2(90, 0), t))
        app.saveRequested = true;
    ImGui::SameLine();
    const char* themes[] = {"Abyss Deep", "Twilight", "Bioluminescent", "Classic"};
    ImGui::SetNextItemWidth(140);
    int ti = themes_.PresetIndex();
    if (ImGui::Combo("##theme", &ti, themes, 4))
        themes_.SetPresetIndex(ti);
}

void TrainerUI::DrawSidebar() {
    const auto& t = themes_.Active();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.f, 2.f));

    for (int i = 0; i < static_cast<int>(functions::Category::Presets); ++i) {
        auto cat = static_cast<functions::Category>(i);
        char buf[48];
        std::snprintf(buf, sizeof(buf), "[%s] %s", functions::CategoryIcon(cat),
                      functions::CategoryShortName(cat));
        if (NavCategoryButton(functions::CategoryName(cat), buf, selected_ == cat, t))
            selected_ = cat;
    }
    if (NavCategoryButton("presets_nav", "[=] Presets", selected_ == functions::Category::Presets, t))
        selected_ = functions::Category::Presets;

    ImGui::PopStyleVar();
    ImGui::Spacing();
    DrawSidebarFooter();
}

void TrainerUI::DrawSidebarFooter() {
    const auto& t = themes_.Active();
    constexpr const char* kSubnauticaBuild = "2026.05.18 (2026 patch, PC/Steam)";
    constexpr const char* kTargetModule = "Subnautica 2 (PC)";

    const float w = ImGui::GetContentRegionAvail().x;
    const ImVec2 linePos = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddLine(linePos, ImVec2(linePos.x + w, linePos.y),
                ImGui::ColorConvertFloat4ToU32(t.panelBorder), 1.f);
    ImGui::Dummy(ImVec2(0.f, 4.f));

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.f, 1.f));
    ImGui::SetWindowFontScale(0.92f);
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + w);
    ImGui::TextColored(t.textDim, "Game");
    ImGui::TextColored(t.text, "Subnautica 2");
    ImGui::TextColored(t.textDim, "Build %s", kSubnauticaBuild);
    ImGui::TextColored(t.textDim, "Module");
    ImGui::TextColored(t.accent, "%s", kTargetModule);
    ImGui::PopTextWrapPos();
    ImGui::SetWindowFontScale(1.f);
    ImGui::PopStyleVar();
}

void TrainerUI::DrawContent(config::AppSettings& app) {
    if (selected_ == functions::Category::Presets) {
        DrawPresetsTab(app);
        return;
    }
    const auto& t = themes_.Active();
    GlowText(functions::CategoryName(selected_), t, 0.85f);
    ImGui::Spacing();

    auto items = reg_.ByCategory(selected_);
    for (auto* fn : items)
        DrawFunction(*fn, app);
}

void TrainerUI::DrawFunction(functions::TrainerFunction& fn, config::AppSettings& app) {
    (void)app;
    const auto& t = themes_.Active();
    ImGui::PushID(fn.id.c_str());
    ImGui::Spacing();

    switch (fn.kind) {
    case functions::FunctionKind::Toggle:
        BioToggle(fn.id.c_str(), &fn.enabled, fn, t, log_, reg_.FakeDepthMeters());
        if (fn.id == "player_invuln" && fn.enabled) {
            ImGui::Indent(56.f);
            ImGui::TextColored(t.textDim, "Shield active (visual)");
            ImGui::Unindent(56.f);
        }
        break;
    case functions::FunctionKind::Slider:
        ImGui::TextColored(t.text, "%s", fn.label.c_str());
        ImGui::Indent(4.f);
        BioSlider(fn.id.c_str(), &fn.value, fn.minValue, fn.maxValue, "%.1f", t);
        ImGui::Unindent(4.f);
        break;
    case functions::FunctionKind::ToggleSlider:
        BioToggle(fn.id.c_str(), &fn.enabled, fn, t, log_, reg_.FakeDepthMeters());
        if (fn.enabled) {
            ImGui::Indent(56.f);
            BioSlider((fn.id + "_sl").c_str(), &fn.value, fn.minValue, fn.maxValue,
                      fn.id == "health" ? "%.0f HP" : "%.1f", t);
            ImGui::Unindent(56.f);
        }
        break;
    case functions::FunctionKind::ToggleBar:
        BioToggle(fn.id.c_str(), &fn.enabled, fn, t, log_, reg_.FakeDepthMeters());
        ImGui::Indent(56.f);
        ImGui::TextColored(t.textDim, "Oxygen level");
        OxygenBar(fn.enabled ? 1.f : fn.value, t);
        ImGui::Unindent(56.f);
        break;
    case functions::FunctionKind::Combo:
        ImGui::TextColored(t.text, "%s", fn.label.c_str());
        if (ImGui::BeginCombo("##c", fn.comboItems.empty() ? "?" : fn.comboItems[fn.comboIndex].c_str())) {
            for (int i = 0; i < static_cast<int>(fn.comboItems.size()); ++i) {
                if (ImGui::Selectable(fn.comboItems[i].c_str(), fn.comboIndex == i))
                    fn.comboIndex = i;
            }
            ImGui::EndCombo();
        }
        break;
    case functions::FunctionKind::TeleportButtons:
        ImGui::TextColored(t.text, "%s", fn.label.c_str());
        for (const auto& lbl : fn.teleportLabels) {
            ImGui::PushID(lbl.c_str());
            if (RippleButton(lbl.c_str(), ImVec2(120, 0), t))
                log_.AddAtmospheric("Travel → " + lbl, true, reg_.FakeDepthMeters());
            ImGui::SameLine();
            ImGui::PopID();
        }
        ImGui::NewLine();
        break;
    case functions::FunctionKind::Button:
        if (RippleButton(fn.label.c_str(), ImVec2(-1, 0), t))
            log_.AddAtmospheric(fn.label, true, reg_.FakeDepthMeters());
        break;
    case functions::FunctionKind::ButtonProgress:
        if (RippleButton(fn.label.c_str(), ImVec2(-1, 0), t))
            fn.fakeProgress = 0.f;
        fn.fakeProgress = std::min(1.f, fn.fakeProgress + ImGui::GetIO().DeltaTime * 0.15f);
        ImGui::ProgressBar(fn.fakeProgress, ImVec2(-1, 0), "Scanning databank...");
        break;
    case functions::FunctionKind::TextFields:
        ImGui::TextColored(t.text, "%s", fn.label.c_str());
        for (size_t i = 0; i < fn.textFieldLabels.size(); ++i) {
            ImGui::TextColored(t.textDim, "%s", fn.textFieldLabels[i].c_str());
            char buf[256];
            std::snprintf(buf, sizeof(buf), "%s", fn.textFieldValues[i].c_str());
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::InputText("##fld", buf, sizeof(buf)))
                fn.textFieldValues[i] = buf;
            ImGui::PopID();
        }
        break;
    case functions::FunctionKind::MoodFilters:
        ImGui::TextColored(t.text, "%s", fn.label.c_str());
        for (int i = 0; i < static_cast<int>(fn.comboItems.size()); ++i) {
            if (RippleButton(fn.comboItems[i].c_str(), ImVec2(100, 0), t))
                log_.Add("Ocean mood → " + fn.comboItems[i]);
            ImGui::SameLine();
        }
        ImGui::NewLine();
        break;
    case functions::FunctionKind::ModSlotList: {
        ImGui::TextColored(t.text, "%s", fn.label.c_str());
        BioSlider(fn.id.c_str(), &fn.value, fn.minValue, fn.maxValue, "%.0f slots", t);
        static const char* mods[] = {"Pressure Adapt", "Silent Swim", "Thermal Weave", "Sonar Ping"};
        for (const char* m : mods) {
            bool dummy = true;
            ImGui::BulletText("%s", m);
            (void)dummy;
        }
        break;
    }
    case functions::FunctionKind::LoreGallery:
        BioToggle(fn.id.c_str(), &fn.enabled, fn, t, log_, reg_.FakeDepthMeters());
        if (fn.enabled) {
            ImGui::TextColored(t.textDim, "Hidden log #447 — \"The abyss remembers.\"");
            ImGui::TextColored(t.textDim, "[photo] Degasi habitat — corrupted");
            ImGui::TextColored(t.textDim, "[note] Unknown alloy sample — inert");
        }
        break;
    default:
        break;
    }

    if (!fn.description.empty()) {
        ImGui::Indent(56.f);
        ImGui::TextColored(t.textDim, "%s", fn.description.c_str());
        ImGui::Unindent(56.f);
    }
    ImGui::Spacing();

    ImGui::PopID();
}

void TrainerUI::DrawMinimap() {
    const auto& t = themes_.Active();
    const float mapW = std::max(80.f, ImGui::GetContentRegionAvail().x);
    const float mapH = std::min(130.f, mapW * 0.72f);

    GlowText("Sector Map", t, 0.7f);
    ImVec2 p = ImGui::GetCursorScreenPos();
    const ImVec2 sz(mapW, mapH);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImVec2 p2(p.x + sz.x, p.y + sz.y);
    dl->AddRectFilled(p, p2, ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0.05f, 0.08f, 0.6f)), 8.f);
    dl->AddRect(p, p2, ImGui::ColorConvertFloat4ToU32(t.panelBorder), 8.f);
    dl->AddCircleFilled(ImVec2(p.x + sz.x * 0.45f, p.y + sz.y * 0.55f), 4.f,
                        ImGui::ColorConvertFloat4ToU32(t.accent), 12);
    dl->AddCircle(ImVec2(p.x + sz.x * 0.45f, p.y + sz.y * 0.55f), 14.f,
                  ImGui::ColorConvertFloat4ToU32(ImVec4(t.accent.x, t.accent.y, t.accent.z, 0.3f)),
                  0, 1.f);
    ImGui::Dummy(sz);
    reg_.SetFakeDepth(200 + static_cast<int>(std::fmod(util::NowSeconds() * 3.0, 800)));
}

void TrainerUI::DrawFakeEchoMap() {
    const auto& t = themes_.Active();
    const float boxW = std::max(80.f, ImGui::GetContentRegionAvail().x);
    const float boxH = 52.f;

    ImGui::Spacing();
    ImGui::TextColored(t.textDim, "Echo Locator");
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    for (int i = 0; i < 5; ++i) {
        const float ax = p.x + 8.f + (boxW - 16.f) * (static_cast<float>(i) / 4.f);
        const float ay = p.y + 14.f + (i % 3) * 12.f;
        dl->AddCircleFilled(ImVec2(ax, ay), 3.f,
                            ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 0.8f, 0.2f, 0.8f)), 8);
    }
    ImGui::Dummy(ImVec2(boxW, boxH));
}

void TrainerUI::DrawHotkeyPanel() {
    // Hotkey hints removed from UI (keys still work)
}

void TrainerUI::DrawLogPanel() {
    // Activity log text removed from UI
}

void TrainerUI::DrawPresetsTab(config::AppSettings& app) {
    const auto& t = themes_.Active();
    GlowText("Presets & Theme", t);
    if (RippleButton("Relaxed Explorer", ImVec2(180, 0), t)) {
        ApplyPreset("Relaxed Explorer");
        app.lastPreset = "Relaxed Explorer";
    }
    ImGui::SameLine();
    if (RippleButton("Speedrunner", ImVec2(180, 0), t)) {
        ApplyPreset("Speedrunner");
        app.lastPreset = "Speedrunner";
    }
    ImGui::SameLine();
    if (RippleButton("Creative Suite", ImVec2(180, 0), t)) {
        ApplyPreset("Creative Suite");
        app.lastPreset = "Creative Suite";
    }
    ImGui::Spacing();
    ImGui::TextColored(t.textDim, "Sound (hover/activate): %s",
                        app.soundEnabled ? "ON (stub)" : "OFF");
    ImGui::Checkbox("Enable ambient audio stub", &app.soundEnabled);
    ImGui::Checkbox("Keep window on top", &app.overlayMode);
}

} // namespace ui
