#pragma once

#include <functional>
#include <string>

struct HWND__;
typedef struct HWND__* HWND;

namespace util {

class Hotkeys {
public:
    void RegisterDefaults(HWND hwnd);
    void Shutdown(HWND hwnd);
    void Poll(HWND owner);
    void SetCallback(std::function<void(const std::string& fnId)> cb) { callback_ = std::move(cb); }
    void ProcessWinMessage(unsigned msg, std::uintptr_t wp);

    static const char* HotkeyLegend();

private:
    std::function<void(const std::string&)> callback_;
};

} // namespace util
