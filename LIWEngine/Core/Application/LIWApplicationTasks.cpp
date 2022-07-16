#include "LIWApplicationTasks.h"
#include "LIWCore.h"

void LIW::LIW_TT_WindowUpdate::Execute()
{
	auto ptrWindow = LIWCore::s_ins.m_window;
	if (!ptrWindow->UpdateWindow()) {
		auto ptrFT_NotifyStop = new LIW_FT_NotifyStop();
		LIWFiberExecutor::m_executor.Submit(ptrFT_NotifyStop);
		m_ptrFrameData->m_singnalEnd = true;
	}
	LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEBEG, 1);
}

void LIW::LIW_TT_EventPoll::Execute()
{
	glfwPollEvents();
	LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEBEG, 1);
}

void LIW::LIW_TT_SwapBuffer::Execute()
{
	auto ptrWindow = LIWCore::s_ins.m_window;
	ptrWindow->SwapBuffer();

	LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEEND, 1);
}
