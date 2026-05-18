#include "util/KeyPoll.h"

#include <windows.h>

namespace util {

bool KeyPoll::Pressed(int vk) {
    if (vk < 0 || vk > 255)
        return false;
    const bool down = (GetAsyncKeyState(vk) & 0x8000) != 0;
    const bool edge = down && !wasDown_[vk];
    wasDown_[vk] = down;
    return edge;
}

} // namespace util
