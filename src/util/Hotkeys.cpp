#include "util/Hotkeys.h"

#include "util/KeyPoll.h"

#include <windows.h>

namespace util {

namespace {
struct Binding {
    int vk;
    const char* id;
};

const Binding kBindings[] = {
    {VK_F1, "player_invuln"},
    {VK_F2, "free_move"},
    {VK_F3, "oxygen"},
    {VK_F4, "aerial_move"},
    {VK_NUMPAD1, "health"},
    {VK_NUMPAD2, "oxygen"},
    {VK_NUMPAD3, "walk_speed"},
    {VK_NUMPAD4, "swim_speed"},
    {VK_NUMPAD5, "vehicle_invuln"},
};

KeyPoll g_keys;
} // namespace

void Hotkeys::RegisterDefaults(HWND hwnd) {
    (void)hwnd;
}

void Hotkeys::Shutdown(HWND hwnd) {
    (void)hwnd;
}

void Hotkeys::Poll(HWND owner) {
    if (!callback_ || !owner)
        return;
    if (GetForegroundWindow() != owner)
        return;
    for (const auto& b : kBindings) {
        if (g_keys.Pressed(b.vk))
            callback_(b.id);
    }
}

void Hotkeys::ProcessWinMessage(unsigned msg, std::uintptr_t wp) {
    (void)msg;
    (void)wp;
}

const char* Hotkeys::HotkeyLegend() {
    return "INSERT show (hidden) / toggle (focused) | HOME hide | F1-Num (focused)";
}

} // namespace util
