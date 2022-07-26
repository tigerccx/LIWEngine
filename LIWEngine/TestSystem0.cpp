#include "TestSystem0.h"
#include "LIWCore.h"
#include "Framework/LIWECSFunctional.h"

void FT_TestSystem0Update::Execute(LIWFiberWorkerPointer thisFiber)
{
	using namespace LIW;

	auto& testComponent0Mngr = LIW_ECS_GetComponentManager(TestComponent0);
	int componentCount = (int)testComponent0Mngr.GetActiveComponentCount();
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(TEST_SYNC_COUNTER_TESTSYSTEM0, componentCount);

	int dispatchSize = 20;

	for (int iBeg = 0, iEnd = dispatchSize; iBeg < componentCount; iBeg = iEnd, iEnd += dispatchSize) {
		auto ptrParamDispatch = liw_new_def<FT_TestSystem0Update_TestComponent0>();
		ptrParamDispatch->m_idxBeg = iBeg;
		ptrParamDispatch->m_idxEnd = min(iEnd, componentCount);
		ptrParamDispatch->m_dt = m_ptrFrameData->m_timeDelta;
		LIWFiberExecutor::m_executor.Submit(ptrParamDispatch);
	}

	LIWFiberExecutor::m_executor.WaitOnSyncCounter(TEST_SYNC_COUNTER_TESTSYSTEM0, thisFiber);

	LIWFiberExecutor::m_executor.DecreaseSyncCounter(TEST_SYNC_COUNTER_SYSTEM, 1);
}

void FT_TestSystem0Update_TestComponent0::Execute(LIWFiberWorkerPointer thisFiber)
{
	using namespace LIW;

	auto& testComponent0Mngr = LIW_ECS_GetComponentManager(TestComponent0);
	for (int i = m_idxBeg; i < m_idxEnd; i++) {
		TestComponent0& component = testComponent0Mngr.m_components[i];
		component.m_float0 += 1.0f;
		auto entity = component.GetEntity();
		liw_objhdl_type transformHdl = LIW_ECS_GetComponentFromEntity(LIWComponent_Transform, entity);
		if (transformHdl != liw_c_nullobjhdl) {
			LIWComponent_Transform& transform = LIW_ECS_GetComponent(LIWComponent_Transform, transformHdl);
			printf("LIWComponent_Transform[%lu] from Entity[%lu] : pos:(%f,%f,%f)\n", transform.GetHandle(), transform.GetEntity(), transform.m_location.x, transform.m_location.y, transform.m_location.z);
		}
		printf("TestComponent[%lu] from Entity[%lu] : %f\n", component.GetHandle(), component.GetEntity(), component.m_float0);
	}
	LIWFiberExecutor::m_executor.DecreaseSyncCounter(TEST_SYNC_COUNTER_TESTSYSTEM0, m_idxEnd - m_idxBeg);
}
