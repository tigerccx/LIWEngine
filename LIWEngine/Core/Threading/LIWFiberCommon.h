#pragma once
#include <functional>
#include <atomic>
#include <list>

#include "Memory/LIWMemory.h"

namespace LIW {
	class LIWFiberMain;
	class LIWFiberWorker;
	class LIWFiberTask;

	typedef LIWPointer<LIWFiberWorker, LIWMem_Static> LIWFiberWorkerPointer;
#define LIWFiberWorkerPointer_NULL LIWFiberWorkerPointer::null
	typedef LIWPointer<LIWFiberTask, LIWMem_Default> LIWFiberTaskPointer;
#define LIWFiberTaskPointer_NULL LIWFiberTaskPointer::null

	enum class LIWFiberState {
		Uninit,
		Idle,
		Running
	};
}

enum LIW_SYNC_COUNTER_RESERVE {
	LIW_SYNC_COUNTER_RESERVE_FRAMEBEG,
	LIW_SYNC_COUNTER_RESERVE_UPDATE,
	LIW_SYNC_COUNTER_RESERVE_RENDER,
	LIW_SYNC_COUNTER_RESERVE_FRAMEEND,
	LIW_SYNC_COUNTER_RESERVE_MEMORY_UPDATE,
	LIW_SYNC_COUNTER_RESERVE_MAX
};

enum LIW_SYNC_COUNTER_RESERVE_EDTR {
	LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW = LIW_SYNC_COUNTER_RESERVE_MAX,
	LIW_SYNC_COUNTER_RESERVE_EDTR_MAX
};