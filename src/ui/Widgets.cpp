#include "ui/Widgets.h"

#include "imgui_internal.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace ui {

namespace {

void DrawBioSwitch(ImDrawList* dl, ImVec2 center, float w, float h, float anim, bool hovered,
                   const themes::OceanTheme& theme) {
    const ImVec2 p(center.x - w * 0.5f, center.y - h * 0.5f);
    const ImVec2 p2(p.x + w, p.y + h);
    const float r = h * 0.5f;

    const ImU32 outer = ImGui::ColorConvertFloat4ToU32(ImVec4(
        theme.accent.x * 0.55f, theme.accent.y * 0.55f, theme.accent.z * 0.55f, hovered ? 0.95f : 0.75f));
    const ImU32 groove = ImGui::ColorConvertFloat4ToU32(ImVec4(0.04f, 0.11f, 0.16f, 0.98f));
    const ImU32 grooveHi = ImGui::ColorConvertFloat4ToU32(ImVec4(0.07f, 0.18f, 0.24f, 0.9f));
    const ImU32 activeFill = ImGui::ColorConvertFloat4ToU32(ImVec4(
        theme.accent.x, theme.accent.y, theme.accent.z, 0.22f + anim * 0.38f));

    dl->AddRectFilled(ImVec2(p.x - 1.f, p.y - 1.f), ImVec2(p2.x + 1.f, p2.y + 1.f), outer, r + 1.f);
    dl->AddRectFilled(p, p2, groove, r);
    dl->AddRect(p, p2, outer, r, 0, 1.8f);
    dl->AddRectFilled(ImVec2(p.x + 2.f, p.y + 2.f), ImVec2(p2.x - 2.f, p2.y - 2.f), grooveHi, r - 2.f);
    if (anim > 0.01f)
        dl->AddRectFilled(p, p2, activeFill, r);

    const float knobR = (h - 8.f) * 0.5f;
    const float travel = w - h;
    const float knobCx = p.x + h * 0.5f + travel * anim;
    const float knobCy = center.y;

    if (hovered) {
        dl->AddCircleFilled(ImVec2(knobCx, knobCy), knobR + 5.f,
                            ImGui::ColorConvertFloat4ToU32(
                                ImVec4(theme.glow.x, theme.glow.y, theme.glow.z, 0.35f)),
                            24);
    }
    dl->AddCircleFilled(ImVec2(knobCx, knobCy), knobR + 1.f,
                        ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 0.2f)), 24);
    dl->AddCircleFilled(ImVec2(knobCx, knobCy), knobR,
                        ImGui::ColorConvertFloat4ToU32(theme.accent), 24);
}

ImVec2 RectCenter(const ImVec2& a, const ImVec2& b) {
    return ImVec2((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f);
}

} // namespace

void GlowText(const char* text, const themes::OceanTheme& theme, float intensity) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImU32 glow = ImGui::ColorConvertFloat4ToU32(ImVec4(
        theme.glow.x, theme.glow.y, theme.glow.z, theme.glow.w * intensity * 0.8f));
    for (int i = 0; i < 3; ++i)
        dl->AddText(ImVec2(pos.x - i, pos.y), glow, text);
    ImGui::TextColored(theme.text, "%s", text);
}

bool BioToggle(const char* id, bool* v, functions::TrainerFunction& fn,
               const themes::OceanTheme& theme, util::LogBuffer& log, int fakeDepth) {
    ImGui::PushID(id);

    const float switchW = 52.f;
    const float switchH = 24.f;
    const float rowH = 30.f;

    float target = *v ? 1.f : 0.f;
    fn.anim += (target - fn.anim) * (1.f - std::exp(-12.f * ImGui::GetIO().DeltaTime));

    bool clicked = false;
    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX;

    if (ImGui::BeginTable("##toggle_row", 2, tableFlags)) {
        ImGui::TableSetupColumn("switch", ImGuiTableColumnFlags_WidthFixed, switchW + 12.f);
        ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextRow(ImGuiTableRowFlags_None, rowH);

        ImGui::TableSetColumnIndex(0);
        ImGui::InvisibleButton("##sw", ImVec2(switchW + 8.f, rowH));
        clicked = ImGui::IsItemClicked();
        const bool hovered = ImGui::IsItemHovered();
        if (clicked) {
            *v = !*v;
            log.AddAtmospheric(fn.label, *v, fakeDepth);
        }
        const ImVec2 swCenter = RectCenter(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        DrawBioSwitch(ImGui::GetWindowDrawList(), swCenter, switchW, switchH, fn.anim, hovered, theme);

        ImGui::TableSetColumnIndex(1);
        ImGui::AlignTextToFramePadding();
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x);
        ImGui::TextColored(theme.text, "%s", fn.label.c_str());
        if (!fn.hotkeyHint.empty()) {
            ImGui::SameLine(0.f, 8.f);
            ImGui::TextColored(theme.textDim, "[%s]", fn.hotkeyHint.c_str());
        }
        ImGui::SameLine(0.f, 12.f);
        ImGui::TextColored(*v ? theme.accent : theme.textDim, "%s", *v ? "ON" : "OFF");
        ImGui::PopTextWrapPos();

        ImGui::EndTable();
    }

    ImGui::PopID();
    return clicked;
}

bool BioSlider(const char* id, float* v, float minV, float maxV, const char* fmt,
               const themes::OceanTheme& theme) {
    ImGui::PushID(id);

    const float width = ImGui::GetContentRegionAvail().x;
    const float trackH = 22.f;
    const float rowH = 36.f;
    const float valueW = 56.f;
    const float trackW = std::max(80.f, width - valueW - 10.f);

    const ImVec2 rowPos = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    const ImVec2 trackMin(rowPos.x, rowPos.y + (rowH - trackH) * 0.5f);
    const ImVec2 trackMax(trackMin.x + trackW, trackMin.y + trackH);

    ImGui::InvisibleButton("##sl_hit", ImVec2(width, rowH));
    const bool hot = ImGui::IsItemHovered();
    const bool active = ImGui::IsItemActive();
    bool changed = false;

    if (ImGui::IsItemActivated() || (active && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.f))) {
        const float mx = ImGui::GetIO().MousePos.x;
        float t = (mx - trackMin.x) / (trackMax.x - trackMin.x);
        t = std::clamp(t, 0.f, 1.f);
        const float nv = minV + t * (maxV - minV);
        if (nv != *v) {
            *v = nv;
            changed = true;
        }
    }

    float t = (*v - minV) / (maxV - minV);
    t = std::clamp(t, 0.f, 1.f);

    const ImU32 trackOuter = ImGui::ColorConvertFloat4ToU32(ImVec4(
        theme.accent.x * 0.5f, theme.accent.y * 0.5f, theme.accent.z * 0.5f, 0.85f));
    const ImU32 trackFill = ImGui::ColorConvertFloat4ToU32(ImVec4(0.04f, 0.11f, 0.16f, 0.98f));
    const ImU32 trackInner = ImGui::ColorConvertFloat4ToU32(ImVec4(0.07f, 0.17f, 0.22f, 0.85f));
    const ImU32 valueFill = ImGui::ColorConvertFloat4ToU32(ImVec4(
        theme.accent.x, theme.accent.y, theme.accent.z, 0.55f));

    dl->AddRectFilled(ImVec2(trackMin.x - 1.f, trackMin.y - 1.f),
                      ImVec2(trackMax.x + 1.f, trackMax.y + 1.f), trackOuter, 9.f);
    dl->AddRectFilled(trackMin, trackMax, trackFill, 8.f);
    dl->AddRect(trackMin, trackMax, trackOuter, 8.f, 0, 2.f);
    dl->AddRectFilled(ImVec2(trackMin.x + 2.f, trackMin.y + 2.f),
                      ImVec2(trackMax.x - 2.f, trackMax.y - 2.f), trackInner, 7.f);

    if (t > 0.002f) {
        const ImVec2 fillMax(trackMin.x + (trackMax.x - trackMin.x) * t, trackMax.y);
        dl->AddRectFilled(trackMin, fillMax, valueFill, 8.f);
    }

    for (int i = 1; i <= 4; ++i) {
        const float tx = trackMin.x + (trackMax.x - trackMin.x) * (static_cast<float>(i) / 5.f);
        dl->AddLine(ImVec2(tx, trackMin.y + 4.f), ImVec2(tx, trackMax.y - 4.f),
                    ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 0.16f)), 1.2f);
    }

    const float grabX = trackMin.x + (trackMax.x - trackMin.x) * t;
    const float grabY = (trackMin.y + trackMax.y) * 0.5f;
    const ImU32 grabGlow = ImGui::ColorConvertFloat4ToU32(ImVec4(
        theme.glow.x, theme.glow.y, theme.glow.z, (hot || active) ? 0.5f : 0.28f));
    const ImU32 grabCore = ImGui::ColorConvertFloat4ToU32(theme.accent);
    const ImU32 grabRing = ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, (hot || active) ? 0.4f : 0.25f));

    dl->AddCircleFilled(ImVec2(grabX, grabY), (hot || active) ? 11.f : 9.f, grabGlow, 24);
    dl->AddCircleFilled(ImVec2(grabX, grabY), 7.f, grabCore, 20);
    dl->AddCircle(ImVec2(grabX, grabY), 8.5f, grabRing, 0, 1.6f);

    char valueBuf[32];
    std::snprintf(valueBuf, sizeof(valueBuf), fmt, *v);
  const ImVec2 valPos(trackMax.x + 10.f, rowPos.y + 6.f);
    dl->AddText(valPos, ImGui::ColorConvertFloat4ToU32(theme.text), valueBuf);

    ImGui::Dummy(ImVec2(width, rowH));
    ImGui::PopID();
    return changed;
}

void OxygenBar(float fill, const themes::OceanTheme& theme) {
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImVec2 sz(ImGui::GetContentRegionAvail().x, 20.f);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImVec2 barEnd(p.x + sz.x, p.y + sz.y);
    dl->AddRectFilled(p, barEnd, ImGui::ColorConvertFloat4ToU32(ImVec4(0.03f, 0.1f, 0.14f, 0.95f)), 6.f);
    dl->AddRect(p, barEnd,
                ImGui::ColorConvertFloat4ToU32(ImVec4(
                    theme.panelBorder.x, theme.panelBorder.y, theme.panelBorder.z, 0.9f)),
                6.f, 0, 1.4f);
    fill = std::clamp(fill, 0.f, 1.f);
    ImU32 fillCol = ImGui::ColorConvertFloat4ToU32(
        ImVec4(theme.accent.x, theme.accent.y, theme.accent.z, 0.8f));
    dl->AddRectFilled(p, ImVec2(p.x + sz.x * fill, p.y + sz.y), fillCol, 6.f);
    ImGui::Dummy(sz);
}

bool NavCategoryButton(const char* id, const char* label, bool selected,
                       const themes::OceanTheme& theme) {
    ImGui::PushID(id);
    const float w = ImGui::GetContentRegionAvail().x;
    const float padX = 8.f;
    const float wrapW = std::max(40.f, w - padX * 2.f);
    const ImVec2 textSize = ImGui::CalcTextSize(label, nullptr, false, wrapW);
    const float h = std::max(28.f, textSize.y + 8.f);

    const ImVec2 rowPos = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##nav", ImVec2(w, h));
    const bool pressed = ImGui::IsItemClicked();
    const bool hovered = ImGui::IsItemHovered();

    const ImVec2 r0 = ImGui::GetItemRectMin();
    const ImVec2 r1 = ImGui::GetItemRectMax();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    ImU32 bg = ImGui::ColorConvertFloat4ToU32(
        selected ? ImVec4(theme.accent.x, theme.accent.y, theme.accent.z, 0.42f)
                 : ImVec4(theme.panel.x, theme.panel.y, theme.panel.z, hovered ? 0.62f : 0.48f));
    ImU32 border = ImGui::ColorConvertFloat4ToU32(
        selected ? ImVec4(theme.accent.x, theme.accent.y, theme.accent.z, 0.85f) : theme.panelBorder);

    dl->AddRectFilled(r0, r1, bg, 8.f);
    dl->AddRect(r0, r1, border, 8.f, 0, selected ? 1.6f : 1.f);

    ImGui::PushClipRect(r0, r1, true);
    ImGui::SetCursorScreenPos(ImVec2(r0.x + padX, r0.y + 6.f));
    ImGui::PushTextWrapPos(r0.x + wrapW + padX);
    ImGui::TextColored(theme.text, "%s", label);
    ImGui::PopTextWrapPos();
    ImGui::PopClipRect();

    ImGui::SetCursorScreenPos(ImVec2(rowPos.x, rowPos.y + h + 2.f));
    ImGui::PopID();
    return pressed;
}

bool RippleButton(const char* label, const ImVec2& size, const themes::OceanTheme& theme) {
    bool pressed = ImGui::Button(label, size);
    if (ImGui::IsItemHovered()) {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 a = ImGui::GetItemRectMin();
        ImVec2 b = ImGui::GetItemRectMax();
        ImU32 r = ImGui::ColorConvertFloat4ToU32(
            ImVec4(theme.glow.x, theme.glow.y, theme.glow.z, 0.2f));
        dl->AddRect(a, b, r, 8.f, 0, 2.f);
    }
    return pressed;
}

} // namespace ui
