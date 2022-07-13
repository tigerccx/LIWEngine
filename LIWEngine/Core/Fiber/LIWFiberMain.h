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
	public:
		friend class LIWFiberWorker;
	public:
		static inline LIWFiberMain* InitThreadMainFiber(int threadID) {
			return new LIWFiberMain(threadID);
		}
		void YieldTo(LIWFiberWorker* fiberOther);
		inline int GetThreadID() const { return m_threadID; }
	private:
		LIWFiberMain(int threadID);
		~LIWFiberMain();
	private:
		LPVOID m_sysFiber;
		const int m_threadID;
	};
#endif 
}

