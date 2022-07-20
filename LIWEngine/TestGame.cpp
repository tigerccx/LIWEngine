#include "TestGame.h"
#include "Editor/LIWEditorTasks.h"

//LIWPointer<TestGame, LIWMem_Static> TestGame::s_ptrGame{};

int TestGame::Initialise()
{
	m_renderer = new TestRenderer(*(m_currentEnvironment->m_window));

	//LIWEntity* entity = LIWEntityManager::instance.GetNewEntity();
	//entity->AddComponent(LIWComponentManager<TestComponent0>::instance.GetNewComponent());
	
	return 0;
}

#include "LIWCore.h"
#include "TestFiberTaskIds.h"
#include "TestSystem0.h"

void FT_TestGameUpdate::Execute(LIWFiberWorkerPointer thisFiber)
{
	using namespace LIW;

	LIWFiberExecutor::m_executor.IncreaseSyncCounter(TEST_SYNC_COUNTER_SYSTEM, 1);
	auto taskUpdateTestSys0 = liw_new_def<FT_TestSystem0Update>();
	taskUpdateTestSys0->m_ptrFrameData = m_ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(taskUpdateTestSys0);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(TEST_SYNC_COUNTER_SYSTEM, thisFiber);

	LIWFiberExecutor::m_executor.IncreaseSyncCounter(TEST_SYNC_COUNTER_SYSTEM, 1);
	auto ptrFT_TestRendererRender = liw_new_def<FT_TestRenderRender>();
	ptrFT_TestRendererRender->m_renderer = m_ptrGame->m_renderer;
	LIWFiberExecutor::m_executor.Submit(ptrFT_TestRendererRender);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(TEST_SYNC_COUNTER_SYSTEM, thisFiber);

	auto ptrFT_EdtrUIDrawBeg = liw_new_def<Editor::LIW_FT_EDTR_UIDrawBeg>();
	ptrFT_EdtrUIDrawBeg->ptrFrameData = m_ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(ptrFT_EdtrUIDrawBeg);
}
