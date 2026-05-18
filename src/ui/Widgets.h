#pragma once

#include "functions/TrainerFunction.h"
#include "themes/Theme.h"
#include "util/LogBuffer.h"

namespace ui {

void GlowText(const char* text, const themes::OceanTheme& theme, float intensity = 1.f);
bool BioToggle(const char* id, bool* v, functions::TrainerFunction& fn, const themes::OceanTheme& theme,
               util::LogBuffer& log, int fakeDepth);
bool BioSlider(const char* id, float* v, float minV, float maxV, const char* fmt,
               const themes::OceanTheme& theme);
void OxygenBar(float fill, const themes::OceanTheme& theme);
bool RippleButton(const char* label, const ImVec2& size, const themes::OceanTheme& theme);
bool NavCategoryButton(const char* id, const char* label, bool selected, const themes::OceanTheme& theme);

} // namespace ui
