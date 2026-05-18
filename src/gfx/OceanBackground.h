#pragma once

#include "themes/Theme.h"

struct ImDrawList;
struct ImVec2;

namespace gfx {

class OceanBackground {
public:
    void Update(float dt, const themes::OceanTheme& theme, int fakeDepthM);
    void Draw(ImDrawList* dl, ImVec2 origin, ImVec2 size, const themes::OceanTheme& theme,
              float globalTime) const;

private:
    struct Particle {
        float x = 0.f, y = 0.f;
        float vx = 0.f, vy = 0.f;
        float size = 2.f;
        float alpha = 0.5f;
        int type = 0; // 0 plankton, 1 bubble
    };
    struct Silhouette {
        float x = 0.f;
        float y = 0.f;
        float speed = 20.f;
        float scale = 1.f;
        float life = 0.f;
        int shape = 0;
    };

    static constexpr int kMaxParticles = 120;
    Particle particles_[kMaxParticles]{};
    Silhouette creature_{};
    float depthPhase_ = 0.f;
    bool initialized_ = false;
};

} // namespace gfx
