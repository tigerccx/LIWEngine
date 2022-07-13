#include "LIWThread.h"
#include "Memory/LIWMemory.h"

namespace LIW {
	LIWThreads LIWThreads::s_ins;
	thread_local LIWThreadLocalData tl_threadData;

	void LIWThreadRegisterID(int threadID)
	{
		tl_threadData.m_threadID = threadID;
	}
	
	int LIWThreadGetID()
	{	
		return tl_threadData.m_threadID;
	}

	void LIWThreadInit()
	{
		// Init mem for thread
		liw_minit_def_thd(LIWThreadGetID());
		liw_minit_static_thd(LIWThreadGetID());
		liw_minit_frame_thd(LIWThreadGetID());
		liw_minit_dframe_thd(LIWThreadGetID());

		//printf("T%d mem inited\n", LIWThreadGetID());
	}

	void LIWThreadCleanup()
	{
		// Cleanup mem for thread
		liw_mclnup_dframe_thd(LIWThreadGetID());
		liw_mclnup_frame_thd(LIWThreadGetID());
		liw_mclnup_static_thd(LIWThreadGetID());
		liw_mclnup_def_thd(LIWThreadGetID());

		//printf("T%d mem cleaned\n", LIWThreadGetID());
	}
}
