#pragma once
#include "LIWFiberThreadPoolSized.h"

#define LIW_FIBER_COUNT size_t(1) << 8
#define LIW_FIBER_AWAKE_COUNT size_t(1) << 10
#define LIW_FIBER_TASK_COUNT size_t(1) << 16
#define LIW_FIBER_SYNC_COUNTER_COUNT size_t(1) << 10

namespace LIW {
	class LIWFiberExecutor
	{
	public:
		typedef LIW::LIWFiberThreadPoolSized<LIW_FIBER_COUNT, LIW_FIBER_AWAKE_COUNT, LIW_FIBER_TASK_COUNT, LIW_FIBER_SYNC_COUNTER_COUNT> fiber_thdpool_type;
	public:
		static fiber_thdpool_type m_executor;
	};
}


