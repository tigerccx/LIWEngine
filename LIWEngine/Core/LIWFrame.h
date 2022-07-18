#pragma once
#include <chrono>
#include <atomic>

#include "Memory/LIWMemory.h"
#include "Time/LIWTime.h"
#include "Threading/LIWFiberExecutor.h"
#include "Threading/LIWFiberTask.h"
#include "Threading/LIWThreadWorkerTask.h"
#include "Threading/LIWMainThreadExecutor.h"

namespace LIW {
	struct LIWFrameData {
		const std::chrono::system_clock::time_point m_timePoint;
		const float m_timeDelta{ 0 };
		const float m_timeFromStart{ 0 };
		bool m_singnalEnd{ false };
	};

	class LIWFrame {
		friend class LIW_FT_FrameBeg;
	public:
		static const size_t GetFrameCount() { return m_frameCount; }
	private:
		static size_t m_frameCount;
	};

	class LIW_FT_FrameBeg final :
		public LIWFiberTask
	{
	public:
		void Execute(LIWFiberWorkerPointer thisFiber) override;
	};
	
	class LIW_FT_FrameEnd final :
		public LIWFiberTask
	{
	public:
		void Execute(LIWFiberWorkerPointer thisFiber) override;
	public:
		LIWPointer<LIWFrameData, LIWMem_Frame> ptrFrameData;
	};

	class LIW_FT_FrameMemoryThdUpdate final :
		public LIWFiberTask
	{
	public:
		void Execute(LIWFiberWorkerPointer thisFiber) override;
	public:
		int m_idxThread{ -1 };
	};
}