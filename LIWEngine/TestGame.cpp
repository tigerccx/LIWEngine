#include "TestGame.h"

int TestGame::Initialise()
{
	m_renderer = new TestRenderer(*(m_currentEnvironment->m_window));

	//LIWEntity* entity = LIWEntityManager::instance.GetNewEntity();
	//entity->AddComponent(LIWComponentManager<TestComponent0>::instance.GetNewComponent());

	LIWGame::instance = this;
	return 0;
}

#include "LIWCore.h"
#include "TestFiberTaskIds.h"
#include "TestSystem0.h"

void FT_TestGameUpdate(LIW_FIBER_RUNNER_PARAM)
{
	using namespace LIW;

	LIWPointer<GameData, LIWMem_Frame> ptrData((liw_hdl_type)param);
	auto ptrFrameData = ptrData->m_hdlFrameData;
	TestGame* const ptrGame = (TestGame*)ptrData->m_game;
	liw_delete_frame<GameData>((liw_hdl_type)param);

	LIWCore::s_ins.m_fiberThreadPool.IncreaseSyncCounter(TEST_SYNC_COUNTER_SYSTEM, 1);
	LIWCore::s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ TestSystem0::FT_Update, (void*)ptrFrameData.get_handle() });
	LIWCore::s_ins.m_fiberThreadPool.WaitOnSyncCounter(TEST_SYNC_COUNTER_SYSTEM, thisFiber);

	LIWCore::s_ins.m_fiberThreadPool.IncreaseSyncCounter(TEST_SYNC_COUNTER_SYSTEM, 1);
	auto ptrTestRenderData = liw_new_frame<TestRenderer::TestRenderData>(TestRenderer::TestRenderData{ ptrFrameData, ptrGame->m_renderer });
	LIWCore::s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ TestRenderer::FT_TestRenderRender, (void*)ptrTestRenderData.get_handle() });
	LIWCore::s_ins.m_fiberThreadPool.WaitOnSyncCounter(TEST_SYNC_COUNTER_SYSTEM, thisFiber);

	liw_delete_frame<GameData>(ptrData);

	LIWCore::s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ LIWCore::LIW_FT_EDTR_UIDrawBeg, (void*)ptrFrameData.get_handle()});
}