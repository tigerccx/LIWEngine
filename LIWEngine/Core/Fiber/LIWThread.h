#pragma once
#include <iostream>
#include <thread>
#include <vector>

#define LIW_THREAD_IDX_MAIN 0

namespace LIW {
	class LIWThreads final{
		friend class LIWCore;
	private:
		int m_threadCount;
	public:
		constexpr inline const int GetThreadCount() const {
			return m_threadCount;
		}
		static LIWThreads s_ins;
	};

	struct LIWThreadLocalData {
		int m_threadID;
	};

	void LIWThreadRegisterID(int threadID);

	int LIWThreadGetID();

	void LIWThreadInit();

	void LIWThreadCleanup();
}