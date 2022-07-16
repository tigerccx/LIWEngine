#pragma once

#include "LIWThreadWorker.h"

#define LIW_WORKER_MAIN_TASK_COUNT size_t(1) << 16

namespace LIW {
	class LIWMainThreadExecutor
	{
	public:
		typedef LIW::LIWThreadWorker<LIW_WORKER_MAIN_TASK_COUNT> main_thdwkr_type;
	public:
		static main_thdwkr_type m_executor;
	};
}

