#pragma once

namespace rfx {

class StopWatch
{
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    void start();
    StopWatch::TimePoint stop();

    [[nodiscard]]
    std::chrono::microseconds getElapsedTime() const;

private:
    TimePoint startTime;
    TimePoint endTime;
};

} // namespace rfx


