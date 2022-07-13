#include "LIWGameTimer.h"

LIW::LIWGameTimer::LIWGameTimer(void)	{
	firstPoint	= std::chrono::high_resolution_clock::now();
	nowPoint	= firstPoint;
	Tick();
}

double	LIW::LIWGameTimer::GetTotalTimeSeconds()	const {
	Timepoint time = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> diff = time - firstPoint;

	return diff.count();
};

double	LIW::LIWGameTimer::GetTotalTimeMSec()		const {
	Timepoint time = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double, std::milli> diff = time - firstPoint;

	return diff.count();
}

void	LIW::LIWGameTimer::Tick() {
	Timepoint latestTime = std::chrono::high_resolution_clock::now();

	std::chrono::duration<float> diff = latestTime - nowPoint;

	nowPoint = latestTime;

	timeDelta = diff.count();
}