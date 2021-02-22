#include "rfx/pch.h"
#include "rfx/common/StopWatch.h"

using namespace rfx;
using namespace std;
using namespace std::chrono;

// ---------------------------------------------------------------------------------------------------------------------

void StopWatch::start()
{
    startTime = Clock::now();
}

// ---------------------------------------------------------------------------------------------------------------------

StopWatch::TimePoint StopWatch::stop()
{
    endTime = Clock::now();

    return endTime;
}

// ---------------------------------------------------------------------------------------------------------------------

microseconds StopWatch::getElapsedTime() const
{
    return duration_cast<microseconds>(endTime - startTime);
}

// ---------------------------------------------------------------------------------------------------------------------
