#pragma once
#include <chrono>
#include <atomic>

struct LIWFrameData {
	const std::chrono::system_clock::time_point m_timePoint;
	const float m_timeDelta{ 0 };
	const float m_timeFromStart{ 0 };
	bool m_singnalEnd{ false };
};