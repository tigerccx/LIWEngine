#pragma once

namespace LIW {
	typedef void(*LIWThreadWorkerRunner)(void* param);

	struct LIWThreadWorkerTask {
		LIWThreadWorkerRunner m_runner = nullptr;
		void* m_param = nullptr;
	};
}

#define LIW_THREADWORKER_RUNNER_DEF(function_name) void function_name(void* param)
#define LIW_THREADWORKER_RUNNER_PARAM void* param