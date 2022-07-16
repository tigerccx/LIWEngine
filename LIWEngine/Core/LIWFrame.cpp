#include "LIWFrame.h"
#include "LIWCore.h"

#include "TestGame.h"
#include "Application/LIWApplicationTasks.h"

size_t LIW::LIWFrame::m_frameCount{ size_t(-1) };

void LIW::LIW_FT_FrameBeg::Execute(LIWFiberWorker* thisFiber)
{
	std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
	const float timeFromStart = float((std::chrono::duration_cast<std::chrono::nanoseconds>(timeNow - LIWTime::m_timeStart)).count()) * 1e-9f;
	const float timeFrame = float((std::chrono::duration_cast<std::chrono::nanoseconds>(timeNow - LIWTime::m_timeFrame)).count()) * 1e-9f;
	LIWTime::m_timeFrame = timeNow;
	LIWFrame::m_frameCount++;

	printf("FrameBeg %llu -- LastFrame %lf(s) \n", LIWFrame::GetFrameCount(), timeFrame);

	auto ptrFrameData = liw_new_frame<LIWFrameData>(
		LIWFrameData{
			timeNow,
			timeFrame,
			timeFromStart,
			false
		});


	// Window update
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEBEG, 2);
	
	auto ptrTT_WindowUpdate = new LIW_TT_WindowUpdate();
	ptrTT_WindowUpdate->m_ptrFrameData = ptrFrameData;
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_WindowUpdate);
	auto ptrTT_EventPoll = new LIW_TT_EventPoll();

	LIWMainThreadExecutor::m_executor.Submit(ptrTT_EventPoll);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEBEG, thisFiber);

	if (ptrFrameData->m_singnalEnd) {
		LIWCore::s_ins.NotifyShutdown();
		return;
	}

	// Kick off the frame
	auto ptrFT_GameUpdate = new FT_TestGameUpdate();
	ptrFT_GameUpdate->m_ptrFrameData = ptrFrameData;
	ptrFT_GameUpdate->m_ptrGame = &LIWCore::s_ins.m_game;
	LIWFiberExecutor::m_executor.Submit(ptrFT_GameUpdate);
}

void LIW::LIW_FT_FrameEnd::Execute(LIWFiberWorker* thisFiber)
{
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEEND, thisFiber);

	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEEND, 1);
	auto ptrTT_SwapBuffer = new LIW_TT_SwapBuffer();
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_SwapBuffer);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEEND, thisFiber);


	liw_delete_frame<LIWFrameData>(ptrFrameData);

	// Note: do all memory operation before memory management frame udpate. 
	//
	// Update memory management
	//
	int threadCount = LIW::LIWThreads::s_ins.GetThreadCount();

	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_MEMORY_UPDATE, threadCount);
	for (size_t i = 0; i < threadCount; i++) {
		auto ptrFT_FrameThdUpdate = new LIW_FT_FrameMemoryThdUpdate();
		ptrFT_FrameThdUpdate->m_idxThread = int(i);
		LIWFiberExecutor::m_executor.Submit(ptrFT_FrameThdUpdate);
	}
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_MEMORY_UPDATE, thisFiber);

	printf("FrameEnd %llu\n", LIWFrame::GetFrameCount());

	auto ptrFT_FrameBeg = new LIW_FT_FrameBeg();
	LIWFiberExecutor::m_executor.Submit(ptrFT_FrameBeg);
}

void LIW::LIW_FT_FrameMemoryThdUpdate::Execute(LIWFiberWorker* thisFiber)
{
	liw_mupdate_def_thd(m_idxThread);
	liw_mupdate_static_thd(m_idxThread);
	liw_mupdate_frame_thd(m_idxThread);
	liw_mupdate_dframe_thd(m_idxThread);
	LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_MEMORY_UPDATE, 1);
}
