#pragma once
#include <chrono>
#include <atomic>

struct LIWFrameData {
	const std::chrono::system_clock::time_point m_timePoint;
	const double m_timeDelta{ 0 };
	const double m_timeFromStart{ 0 };
	bool m_singnalEnd{ false };
};