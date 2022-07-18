#pragma once
#include "LIWFiberCommon.h"

namespace LIW {
//
// Win32
//
#ifdef _WIN32
#include <windows.h>

	class LIWFiberMain final
	{
		friend class LIWFiberWorker;
		//template<uint64_t, uint64_t, uint64_t, uint32_t>
		//friend class LIWFiberThreadPoolSized;
	public:
		static inline LIWFiberMain* InitThreadMainFiber(int threadID) {
			return new LIWFiberMain(threadID);
		}
		void YieldTo(LIWFiberWorkerPointer fiberOther);
		inline int GetThreadID() const { return m_threadID; }
	private:
		LIWFiberMain(int threadID);
		~LIWFiberMain();
	private:
		LPVOID m_sysFiber;
		const int m_threadID;
	public:
		//std::atomic<uint32_t> m_counter = UINT32_MAX;
		uint32_t m_counter = UINT32_MAX;
	};
#endif 
}

