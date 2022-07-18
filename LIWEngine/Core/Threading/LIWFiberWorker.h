#pragma once
#include "LIWFiberCommon.h"
#include "LIWFiberTask.h"
#include "Memory/LIWMemory.h"

/*
* Here is a description of the standard API of LIWFiberWorker.
* All platform-specific implementation should follow this api layout.
*/
/*

*/

#include <atomic>
static std::atomic<int> howmanytimes = 0;

namespace LIW {
//
// Win32
//
#ifdef _WIN32
#include <windows.h>

	class LIWFiberWorker final
	{
		friend class LIWFiberMain;
	public:
		LIWFiberWorker();
		LIWFiberWorker(int id);
		~LIWFiberWorker();
		LIWFiberWorker(const LIWFiberWorker& other) = delete;
		LIWFiberWorker(LIWFiberWorker&& other) = default;
		LIWFiberWorker& operator=(const LIWFiberWorker& other) = delete;
		LIWFiberWorker& operator=(LIWFiberWorker&& other) = default;

		//Set the run task for fiber
		inline void SetRunTask(LIWFiberTaskPointer task) {
			m_curTask = task;
			m_state = LIWFiberState::Idle;
		}
		//Set the main fiber when obtained by another fiber
		inline void SetMainFiber(LIWFiberMain* fiberMain) { m_fiberMain = fiberMain; }
		//A loop which will yield after finishing runnning a run function
		void Run();
		//Stop worker fiber
		inline void Stop() {
			m_isRunning = false;
		}
		//Yield
		inline void YieldToMain();
		//Yield to a specific fiber
		inline void YieldTo(LIWFiberWorkerPointer fiberYieldTo) {
			record.fetch_sub(1);
			SwitchToFiber(fiberYieldTo->m_sysFiber);
		}
		//Get current state of the fiber
		inline LIWFiberState GetState() const { return m_state; } 

	public:
		LIWFiberMain* m_fiberMain = nullptr; // Current main fiber of the thread this fiber is running on
	//private:
		LIWFiberState m_state = LIWFiberState::Uninit; // State of this fiber
		LIWFiberTaskPointer m_curTask = LIWFiberTaskPointer_NULL; // Current task of this fiber
		int m_id = -1; // ID of the fiber
		bool m_isRunning = true; // Is this fiber still running? (Has it not been terminated?) 
	public:
		std::atomic<size_t> record{ 0 };

	private:
		static void __stdcall InternalFiberRun(LPVOID param) {
			LIWFiberWorkerPointer thisFiber = LIWFiberWorkerPointer((liw_hdl_type)param);
			howmanytimes.fetch_add(1);
			thisFiber->Run();
		}
	private:
		LPVOID m_sysFiber;
	};

#endif 
}



