#include "ui/WindowChrome.h"

#include "imgui.h"

#include <algorithm>
#include <windows.h>

namespace ui {

HWND GetMainNativeWindow() {
    if (ImGuiViewport* vp = ImGui::GetMainViewport())
        return static_cast<HWND>(vp->PlatformHandleRaw);
    return nullptr;
}

namespace {

// Manual move avoids re-entering the system drag loop every frame (causes jitter).
struct WindowDragState {
    bool active = false;
    POINT lastCursor{};
};

WindowDragState& DragState() {
    static WindowDragState state;
    return state;
}

void UpdateWindowDrag(HWND hwnd) {
    if (!hwnd)
        return;

    POINT cur{};
    if (!GetCursorPos(&cur))
        return;

    WindowDragState& drag = DragState();
    if (!drag.active) {
        drag.active = true;
        drag.lastCursor = cur;
        return;
    }

    const int dx = cur.x - drag.lastCursor.x;
    const int dy = cur.y - drag.lastCursor.y;
    drag.lastCursor = cur;
    if (dx == 0 && dy == 0)
        return;

    RECT rect{};
    if (!GetWindowRect(hwnd, &rect))
        return;
    SetWindowPos(hwnd, nullptr, rect.left + dx, rect.top + dy, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void EndWindowDrag() {
    DragState().active = false;
}

bool TitleBarButton(const char* id, const char* label, float w, float h, const themes::OceanTheme& theme,
                    bool danger) {
    ImGui::PushID(id);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
    ImVec4 base = danger ? ImVec4(0.35f, 0.08f, 0.1f, 0.55f)
                         : ImVec4(theme.panel.x, theme.panel.y, theme.panel.z, 0.55f);
    ImVec4 hover = danger ? ImVec4(0.75f, 0.15f, 0.2f, 0.85f)
                          : ImVec4(theme.accent.x, theme.accent.y, theme.accent.z, 0.35f);
    ImGui::PushStyleColor(ImGuiCol_Button, base);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, hover);
    ImGui::PushStyleColor(ImGuiCol_Text, theme.text);
    const bool pressed = ImGui::Button(label, ImVec2(w, h));
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();
    ImGui::PopID();
    return pressed;
}

} // namespace

bool DrawWindowTitleBar(const themes::OceanTheme& theme, float width, float height) {
    HWND hwnd = GetMainNativeWindow();
    const float btnW = 36.f;
    const float dragW = std::max(60.f, width - btnW * 2.f - 10.f);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.f, 4.f));

    ImGui::InvisibleButton("##wnd_drag", ImVec2(dragW, height));
    if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        UpdateWindowDrag(hwnd);
    else
        EndWindowDrag();
    if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

    ImGui::SameLine(0.f, 4.f);
    if (TitleBarButton("min", "-", btnW, height, theme, false) && hwnd)
        ShowWindow(hwnd, SW_MINIMIZE);

    ImGui::SameLine(0.f, 2.f);
    const bool close = TitleBarButton("close", "X", btnW, height, theme, true);
    if (close && hwnd)
        PostMessageW(hwnd, WM_CLOSE, 0, 0);

    ImGui::PopStyleVar(2);
    return close;
}

} // namespace ui
