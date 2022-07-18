#include "TestSystem0.h"
#include "LIWCore.h"


void FT_TestSystem0Update::Execute(LIWFiberWorkerPointer thisFiber)
{
	using namespace LIW;

	auto& testComponent0Mngr = LIWComponentManager<TestComponent0>::s_ins;
	int componentCount = testComponent0Mngr.GetComponentCount();
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(TEST_SYNC_COUNTER_TESTSYSTEM0, componentCount);

	int dispatchSize = 20;

	for (int iBeg = 0, iEnd = dispatchSize; iBeg < componentCount; iBeg = iEnd, iEnd += dispatchSize) {
		auto ptrParamDispatch = liw_new_def<FT_TestSystem0Update_TestComponent0>();
		ptrParamDispatch->m_idxBeg = iBeg;
		ptrParamDispatch->m_idxEnd = iEnd;
		ptrParamDispatch->m_dt = m_ptrFrameData->m_timeDelta;
		LIWFiberExecutor::m_executor.Submit(ptrParamDispatch);
	}

	LIWFiberExecutor::m_executor.WaitOnSyncCounter(TEST_SYNC_COUNTER_TESTSYSTEM0, thisFiber);

	LIWFiberExecutor::m_executor.DecreaseSyncCounter(TEST_SYNC_COUNTER_SYSTEM, 1);
}

void FT_TestSystem0Update_TestComponent0::Execute(LIWFiberWorkerPointer thisFiber)
{
	using namespace LIW;

	auto& testComponent0Mngr = LIWComponentManager<TestComponent0>::s_ins;
	for (int i = m_idxBeg; i < m_idxEnd; i++) {
		TestComponent0& component = *testComponent0Mngr.m_list[i];
		component.m_float0 += m_dt;
		printf("%llu : %f", component.GetID(), component.m_float0);
	}
	LIWFiberExecutor::m_executor.DecreaseSyncCounter(TEST_SYNC_COUNTER_TESTSYSTEM0, m_idxEnd - m_idxBeg);
}
