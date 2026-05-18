#pragma once

#include "themes/Theme.h"

struct ImGuiStyle;

namespace themes {

class ThemeManager {
public:
    ThemeManager();

    Preset Current() const { return current_; }
    void Set(Preset p);
    void Next();
    const OceanTheme& Active() const { return active_; }

    void ApplyImGuiStyle(ImGuiStyle& style) const;
    int PresetIndex() const { return static_cast<int>(current_); }
    void SetPresetIndex(int i);

private:
    Preset current_ = Preset::AbyssDeep;
    OceanTheme active_;
};

} // namespace themes
