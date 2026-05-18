#include "util/LogBuffer.h"

#include <cstdio>
#include <random>

namespace util {

void LogBuffer::Add(const std::string& line) {
    lines_.push_back(line);
    while (lines_.size() > kMaxLines)
        lines_.pop_front();
}

void LogBuffer::AddAtmospheric(const std::string& feature, bool activated, int fakeDepthM) {
    char buf[256];
    if (fakeDepthM >= 0) {
        std::snprintf(buf, sizeof(buf), "[%s] %s — depth %dm",
                      activated ? "ON" : "OFF", feature.c_str(), fakeDepthM);
    } else {
        std::snprintf(buf, sizeof(buf), "[%s] %s",
                      activated ? "ON" : "OFF", feature.c_str());
    }
    Add(buf);
}

void LogBuffer::MaybeLeviathanEvent(double timeSec) {
    if (timeSec - lastLeviathan_ < 45.0)
        return;
    static thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> chance(0.f, 1.f);
    if (chance(rng) > 0.012f)
        return;
    lastLeviathan_ = timeSec;
    static const char* messages[] = {
        "...a distant call resonates through the trench.",
        "Bioluminescent plankton swirls past the viewport.",
        "Pressure sensors spike — then settle.",
        "Something vast moves in the black below.",
        "Echo ping: unknown signature fading north-east.",
    };
    std::uniform_int_distribution<int> pick(0, 4);
    Add(std::string(">> ") + messages[pick(rng)]);
}

} // namespace util
