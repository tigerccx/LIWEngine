#include "LIWCore.h"
#include "LIWStandardSystem.h"

void FT_SystemUpdate(LIW_FIBER_RUNNER_PARAM) {
	using namespace LIW;

	StandardSystemData* ptrData = (StandardSystemData*)liw_maddr_frame((liw_hdl_type)param);
	LIWFrameData* ptrFrameData = (LIWFrameData*)liw_maddr_frame(ptrData->m_hdlFrameData);
	LIWStandardSystem* const ptrSys = ptrData->m_system;
	liw_delete_frame<GameData>((liw_hdl_type)param);

	ptrSys->Update(ptrFrameData->m_timeDelta);

	LIWCore::s_ins.m_fiberThreadPool.DecreaseSyncCounter(IDX_SYNCCOUNTER_STANDARDSYS_UPDATE);
}

void FT_SystemRenderUpdate(LIW_FIBER_RUNNER_PARAM) {
	using namespace LIW;

	StandardSystemData* ptrData = (StandardSystemData*)liw_maddr_frame((liw_hdl_type)param);
	LIWFrameData* ptrFrameData = (LIWFrameData*)liw_maddr_frame(ptrData->m_hdlFrameData);
	LIWStandardSystem* const ptrSys = ptrData->m_system;
	liw_delete_frame<GameData>((liw_hdl_type)param);

	ptrSys->RenderUpdate(ptrFrameData->m_timeDelta);

	LIWCore::s_ins.m_fiberThreadPool.DecreaseSyncCounter(IDX_SYNCCOUNTER_STANDARDSYS_RENDERUPDATE);
}