#pragma once

#include "imgui.h"

namespace themes {

enum class Preset {
    AbyssDeep,
    Twilight,
    Bioluminescent,
    Classic,
    Count
};

struct OceanTheme {
    const char* name = "Abyss Deep";
    ImVec4 bgDeep = ImVec4(0.02f, 0.05f, 0.12f, 0.92f);
    ImVec4 bgShallow = ImVec4(0.05f, 0.18f, 0.28f, 0.85f);
    ImVec4 accent = ImVec4(0.2f, 0.85f, 0.78f, 1.f);
    ImVec4 accentDim = ImVec4(0.12f, 0.45f, 0.52f, 1.f);
    ImVec4 text = ImVec4(0.75f, 0.92f, 0.9f, 0.95f);
    ImVec4 textDim = ImVec4(0.45f, 0.65f, 0.68f, 0.75f);
    ImVec4 glow = ImVec4(0.1f, 0.9f, 0.85f, 0.35f);
    ImVec4 panel = ImVec4(0.04f, 0.12f, 0.18f, 0.72f);
    ImVec4 panelBorder = ImVec4(0.15f, 0.55f, 0.6f, 0.45f);
    float waveSpeed = 0.35f;
    float particleDensity = 1.f;
};

OceanTheme ThemeFor(Preset p);
const char* PresetName(Preset p);

} // namespace themes
