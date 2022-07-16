#pragma once
#include <chrono>

namespace LIW {
	class LIWTime
	{
		friend class LIWCore;
		friend class LIW_FT_FrameBeg;
	public:
		typedef std::chrono::system_clock::time_point timepoint_type;
	public:
		static const timepoint_type& GetStartTime() { return m_timeStart; }
		static const timepoint_type& GetFrameTime() { return m_timeFrame; }
	private:
		static timepoint_type m_timeStart;
		static timepoint_type m_timeFrame;
	};
}


