#include "rfx/pch.h"
#include "rfx/core/Timer.h"

using namespace rfx;
using namespace std;
using namespace std::chrono;

// ---------------------------------------------------------------------------------------------------------------------

void Timer::start()
{
    startTime = high_resolution_clock::now();
    previousTime = startTime;
    stopped = false;
}

// ---------------------------------------------------------------------------------------------------------------------

void Timer::stop()
{
    if (!stopped)
    {
        stopTime = high_resolution_clock::now();
        stopped = true;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Timer::tick()
{
    if (stopped)
    {
        return;
    }
    
    time_point currentTime = high_resolution_clock::now();
    elapsedTime = static_cast<float>(1e-9 * duration_cast<nanoseconds>(currentTime - previousTime).count());
    previousTime = currentTime;
}

// ---------------------------------------------------------------------------------------------------------------------

float Timer::getElapsedTime() const
{
    return elapsedTime;
}

// ---------------------------------------------------------------------------------------------------------------------
