#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include <cmath>

namespace ui {

// Smooth scroll: lerps ImGui child scroll toward target each frame.
inline void SmoothScrollY(const char* id, float smoothness = 14.f) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (!window)
        return;
    ImGuiID sid = ImGui::GetID(id);
    float* target = ImGui::GetStateStorage()->GetFloatRef(sid, window->Scroll.y);
    float* init = ImGui::GetStateStorage()->GetFloatRef(sid + 1, 0.f);
    if (*init < 0.5f) {
        *target = window->Scroll.y;
        *init = 1.f;
    }
    if (ImGui::IsWindowHovered() && ImGui::GetIO().MouseWheel != 0.f)
        *target = window->Scroll.y;
    else if (ImGui::IsWindowHovered())
        *target -= ImGui::GetIO().MouseWheel * 42.f;

    float dt = ImGui::GetIO().DeltaTime;
    float t = 1.f - std::exp(-smoothness * dt);
    window->Scroll.y += (*target - window->Scroll.y) * t;
    if (std::fabs(window->Scroll.y - *target) < 0.5f)
        window->Scroll.y = *target;
}

} // namespace ui
