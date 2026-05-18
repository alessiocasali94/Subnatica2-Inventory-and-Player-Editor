#include "gfx/OceanBackground.h"

#include "imgui.h"

#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace gfx {

void OceanBackground::Update(float dt, const themes::OceanTheme& theme, int fakeDepthM) {
    if (!initialized_) {
        initialized_ = true;
        for (int i = 0; i < kMaxParticles; ++i) {
            particles_[i].x = static_cast<float>(rand() % 1000) / 1000.f;
            particles_[i].y = static_cast<float>(rand() % 1000) / 1000.f;
            particles_[i].type = (rand() % 5 == 0) ? 1 : 0;
            particles_[i].size = particles_[i].type ? 2.f + (rand() % 8) : 1.f + (rand() % 3);
            particles_[i].alpha = 0.15f + (rand() % 50) / 100.f;
            particles_[i].vx = (static_cast<float>(rand() % 100) - 50.f) * 0.0002f;
            particles_[i].vy = particles_[i].type ? -0.02f - (rand() % 30) / 1000.f
                                                    : 0.01f + (rand() % 20) / 1000.f;
        }
        creature_.life = 0.f;
    }

    depthPhase_ += dt * theme.waveSpeed * 0.15f;
    const float density = theme.particleDensity;

    for (int i = 0; i < kMaxParticles; ++i) {
        auto& p = particles_[i];
        p.x += p.vx * dt * 60.f * density;
        p.y += p.vy * dt * 60.f;
        if (p.y < -0.05f)
            p.y = 1.05f;
        if (p.y > 1.05f)
            p.y = -0.05f;
        if (p.x < -0.05f)
            p.x = 1.05f;
        if (p.x > 1.05f)
            p.x = -0.05f;
    }

    (void)fakeDepthM;
    creature_.life -= dt;
    if (creature_.life <= 0.f && (rand() % 800) == 0) {
        creature_.life = 8.f + (rand() % 12);
        creature_.x = -0.15f;
        creature_.y = 0.2f + (rand() % 60) / 100.f;
        creature_.speed = 25.f + (rand() % 40);
        creature_.scale = 0.6f + (rand() % 80) / 100.f;
        creature_.shape = rand() % 3;
    }
    if (creature_.life > 0.f)
        creature_.x += creature_.speed * dt / 800.f;
}

void OceanBackground::Draw(ImDrawList* dl, ImVec2 origin, ImVec2 size,
                           const themes::OceanTheme& theme, float globalTime) const {
    const float depthT = 0.5f + 0.5f * std::sin(depthPhase_);
    const ImU32 top = ImGui::ColorConvertFloat4ToU32(theme.bgShallow);
    const ImU32 bot = ImGui::ColorConvertFloat4ToU32(
        ImVec4(theme.bgDeep.x, theme.bgDeep.y, theme.bgDeep.z, theme.bgDeep.w * (0.85f + depthT * 0.1f)));

    dl->AddRectFilledMultiColor(origin, ImVec2(origin.x + size.x, origin.y + size.y), top, top, bot, bot);

    // Caustic wave bands
    for (int w = 0; w < 4; ++w) {
        float yOff = size.y * (0.15f * w + 0.08f * std::sin(globalTime * theme.waveSpeed + w * 1.7f));
        ImU32 waveCol = ImGui::ColorConvertFloat4ToU32(
            ImVec4(theme.glow.x, theme.glow.y, theme.glow.z, 0.03f + 0.02f * w));
        dl->AddRectFilled(ImVec2(origin.x, origin.y + yOff),
                          ImVec2(origin.x + size.x, origin.y + yOff + 28.f), waveCol);
    }

  // Parallax light rays
    for (int r = 0; r < 3; ++r) {
        float rx = origin.x + size.x * (0.2f * r + 0.1f * std::sin(globalTime * 0.2f + r));
        ImU32 ray = ImGui::ColorConvertFloat4ToU32(
            ImVec4(theme.accent.x, theme.accent.y, theme.accent.z, 0.02f));
        dl->AddTriangleFilled(
            ImVec2(rx, origin.y),
            ImVec2(rx + 80.f, origin.y),
            ImVec2(rx + 40.f, origin.y + size.y * 0.7f), ray);
    }

    // Particles
    for (int i = 0; i < kMaxParticles; ++i) {
        const auto& p = particles_[i];
        ImVec2 pos(origin.x + p.x * size.x, origin.y + p.y * size.y);
        ImU32 col = ImGui::ColorConvertFloat4ToU32(ImVec4(
            theme.accent.x, theme.accent.y, theme.accent.z,
            p.alpha * (p.type ? 0.35f : 0.55f) * theme.particleDensity));
        if (p.type)
            dl->AddCircleFilled(pos, p.size, col, 12);
        else
            dl->AddCircleFilled(pos, p.size * 0.6f, col, 8);
    }

    // Creature silhouette
    if (creature_.life > 0.f) {
        float cx = origin.x + creature_.x * size.x;
        float cy = origin.y + creature_.y * size.y;
        float s = 80.f * creature_.scale;
        ImU32 sil = IM_COL32(0, 0, 0, 55);
        if (creature_.shape == 0) {
#if IMGUI_VERSION_NUM >= 18900
            dl->AddEllipseFilled(ImVec2(cx, cy), ImVec2(s * 1.4f, s * 0.35f), sil, 0.f, 24);
#else
            dl->AddNgonFilled(ImVec2(cx, cy), s * 0.9f, sil, 24);
#endif
            dl->AddTriangleFilled(ImVec2(cx - s, cy), ImVec2(cx - s * 1.6f, cy - s * 0.2f),
                                  ImVec2(cx - s * 1.5f, cy + s * 0.15f), sil);
        } else if (creature_.shape == 1) {
            dl->AddBezierCubic(ImVec2(cx - s, cy), ImVec2(cx - s * 0.3f, cy - s),
                               ImVec2(cx + s * 0.5f, cy + s * 0.3f), ImVec2(cx + s * 1.2f, cy),
                               sil, 3.f, 24);
        } else {
            dl->AddRectFilled(ImVec2(cx - s, cy - s * 0.15f), ImVec2(cx + s * 0.8f, cy + s * 0.15f),
                              sil, s * 0.2f);
        }
    }
}

} // namespace gfx
