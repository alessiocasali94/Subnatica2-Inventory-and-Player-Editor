#pragma once

namespace util {

// Edge-triggered key poll (no RegisterHotKey — fewer AV heuristics).
class KeyPoll {
public:
    bool Pressed(int vk);

private:
    bool wasDown_[256]{};
};

} // namespace util
