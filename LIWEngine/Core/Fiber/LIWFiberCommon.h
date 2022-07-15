#pragma once
#include <functional>
#include <atomic>
#include <list>

namespace LIW {
	class LIWFiberMain;
	class LIWFiberWorker;

	enum class LIWFiberState {
		Uninit,
		Idle,
		Running
	};

	typedef void(*LIWFiberRunner)(LIWFiberWorker* thisFiber, void* param);
}

enum LIW_SYNC_COUNTER_RESERVE {
	LIW_SYNC_COUNTER_RESERVE_FRAMEBEG,
	LIW_SYNC_COUNTER_RESERVE_RENDER,
	LIW_SYNC_COUNTER_RESERVE_FRAMEEND,
	LIW_SYNC_COUNTER_RESERVE_MEMORY_UPDATE,
	LIW_SYNC_COUNTER_RESERVE_MAX
};

enum LIW_SYNC_COUNTER_RESERVE_EDTR {
	LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW = LIW_SYNC_COUNTER_RESERVE_MAX,
	LIW_SYNC_COUNTER_RESERVE_EDTR_MAX
};

#define LIW_FIBER_RUNNER_DEF(function_name) void function_name(LIW::LIWFiberWorker* thisFiber, void* param)
#define LIW_FIBER_RUNNER_PARAM LIW::LIWFiberWorker* thisFiber, void* param