#pragma once

#include <deque>
#include <string>

namespace util {

class LogBuffer {
public:
    static constexpr size_t kMaxLines = 48;

    void Add(const std::string& line);
    void AddAtmospheric(const std::string& feature, bool activated, int fakeDepthM = -1);
    void MaybeLeviathanEvent(double timeSec);
    const std::deque<std::string>& Lines() const { return lines_; }

private:
    std::deque<std::string> lines_;
    double lastLeviathan_ = 0.0;
};

} // namespace util
