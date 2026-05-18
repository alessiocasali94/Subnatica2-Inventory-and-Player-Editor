#pragma once

#include <chrono>

namespace util {

inline double NowSeconds() {
    using clock = std::chrono::steady_clock;
    static const auto start = clock::now();
    const auto elapsed = clock::now() - start;
    return std::chrono::duration<double>(elapsed).count();
}

} // namespace util
