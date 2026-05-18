#pragma once

#include "themes/Theme.h"

struct HWND__;
typedef struct HWND__* HWND;

namespace ui {

// Title bar: drag area + minimize + close. Returns true if close was pressed.
bool DrawWindowTitleBar(const themes::OceanTheme& theme, float width, float height = 34.f);

HWND GetMainNativeWindow();

} // namespace ui
