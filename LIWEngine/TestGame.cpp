#include "TestGame.h"
#include "Editor/LIWEditorTasks.h"

#include "Framework/LIWECSFunctional.h"

int TestGame::Initialise()
{
	TestGlobal::s_renderer = liw_new_static<TestRenderer>(*(m_currentEnvironment->m_window));
	
	LIWDArray<liw_objhdl_type> components;
	LIWDArray<LIWEntity> entities;

	LIW_ECS_FetchEntities(entities, 10);
	LIW_ECS_CreateComponents(TestComponent0, components, 10);

	for (int i = 0; i < 10; i++) {
		auto& component = LIW_ECS_GetComponent(TestComponent0, components[i]);
		component.m_float0 = i + 0.7f;
		LIW_ECS_AttachComponentToEntity(TestComponent0, components[i], entities[i]);
	}

	LIW_ECS_ApplyChangeOnComponentManager(TestComponent0);

	return 0;
}

int TestGame::CleanUp()
{
	liw_delete(TestGlobal::s_renderer);
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
	LIWFiberExecutor::m_executor.Submit(ptrFT_TestRendererRender);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(TEST_SYNC_COUNTER_SYSTEM, thisFiber);

	LIW_ECS_ApplyChangeOnComponentManager(TestComponent0);

	auto ptrFT_EdtrUIDrawBeg = liw_new_def<Editor::LIW_FT_EDTR_UIDrawBeg>();
	ptrFT_EdtrUIDrawBeg->ptrFrameData = m_ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(ptrFT_EdtrUIDrawBeg);
}
