#include "LIWGame.h"

int LIW::LIWGame::Initialise()
{
	return 0;
}

int LIW::LIWGame::Update(LIWPointer<LIW::LIWFrameData, LIWMem_Frame> frameData, LIWFiberWorkerPointer thisFiber)
{
	return 0;
}

int LIW::LIWGame::CleanUp()
{
	return 0;
}

#include "LIWGlobal.h"
#include "Editor/LIWEditorTasks.h"

void LIW_FT_GameUpdate::Execute(LIWFiberWorkerPointer thisFiber)
{
	LIWGlobal::GetGame()->Update(m_ptrFrameData, thisFiber);

#ifdef LIW_ENABLE_EDITOR
	// Kick off editor
	auto ptrFT_EdtrUIDrawBeg = liw_new_def<Editor::LIW_FT_EDTR_UIDrawBeg>();
	ptrFT_EdtrUIDrawBeg->ptrFrameData = m_ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(ptrFT_EdtrUIDrawBeg);
#else
	// Straight to end of frame
	auto ptrFT_FrameEnd = liw_new_def<LIW_FT_FrameEnd>();
	ptrFT_FrameEnd->ptrFrameData = m_ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(ptrFT_FrameEnd);
#endif //LIW_ENABLE_EDITOR
}