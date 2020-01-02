#pragma once

namespace rfx
{
	
class Timer
{
public:
	void start();
	void stop();
	void tick();

	float getStartTime() const;
	float getElapsedTime() const;

private:
	bool stopped = true;
	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point stopTime;
	std::chrono::high_resolution_clock::time_point previousTime;
	float elapsedTime = 0.0f;
};

} // namespace rfx


