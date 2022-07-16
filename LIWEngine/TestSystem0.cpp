#include "TestSystem0.h"
#include "LIWCore.h"


void TestSystem0::FT_TestComponent0Update(LIW_FIBER_RUNNER_PARAM)
{
	using namespace LIW;

	auto ptrParam = LIWPointer<TestComponent0UpdateParam, LIWMem_Frame>((liw_hdl_type)param);

	auto& testComponent0Mngr = LIWComponentManager<TestComponent0>::s_ins;
	const int idxBeg = ptrParam->m_idxBeg;
	const int idxEnd = ptrParam->m_idxEnd;
	for (int i = idxBeg; i < idxEnd; i++) {
		TestComponent0& component = *testComponent0Mngr.m_list[i];
		component.m_float0 += ptrParam->m_dt;
		printf("%llu : %f", component.GetID(), component.m_float0);
	}
	LIWCore::s_ins.m_fiberThreadPool.DecreaseSyncCounter(TEST_SYNC_COUNTER_TESTSYSTEM0, idxEnd - idxBeg);
}

void TestSystem0::FT_Update(LIW_FIBER_RUNNER_PARAM)
{
	using namespace LIW;

	auto ptrParam = LIWPointer<LIWFrameData, LIWMem_Frame>((liw_hdl_type)param);

	auto& testComponent0Mngr = LIWComponentManager<TestComponent0>::s_ins;
	int componentCount = testComponent0Mngr.GetComponentCount();
	LIWCore::s_ins.m_fiberThreadPool.IncreaseSyncCounter(TEST_SYNC_COUNTER_TESTSYSTEM0, componentCount);

	int dispatchSize = 20;

	for (int iBeg = 0, iEnd = dispatchSize; iBeg < componentCount; iBeg = iEnd, iEnd += dispatchSize) {
		auto ptrParamDispatch = liw_new_frame<TestComponent0UpdateParam>(TestComponent0UpdateParam{ iBeg,
																									iEnd ,
																									ptrParam->m_timeDelta });
		LIWCore::s_ins.m_fiberThreadPool.Submit(new LIWFiberTask(LIWFiberTask{ FT_TestComponent0Update, (void*)ptrParamDispatch.get_handle() }));
	}

	LIWCore::s_ins.m_fiberThreadPool.WaitOnSyncCounter(TEST_SYNC_COUNTER_TESTSYSTEM0, thisFiber);

	LIWCore::s_ins.m_fiberThreadPool.DecreaseSyncCounter(TEST_SYNC_COUNTER_SYSTEM, 1);
}
