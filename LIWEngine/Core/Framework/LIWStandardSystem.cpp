#include "LIWCore.h"
#include "LIWStandardSystem.h"

/// <summary>
/// Envoke the Update of a standard system. 
/// </summary>
/// <param name=""> StandardSystemData handle(frame) </param>
void FT_SystemUpdate(LIW_FIBER_RUNNER_PARAM) {
	using namespace LIW;

	//auto ptrStdSysData = LIWPointer<StandardSystemData, LIWMem_Frame>((liw_hdl_type)param);
	//auto ptrFrameData = ptrStdSysData->m_hdlFrameData;
	//LIWStandardSystem* const ptrSys = ptrStdSysData->m_system;
	//liw_delete_frame<StandardSystemData>(ptrStdSysData);

	//ptrSys->Update(ptrFrameData->m_timeDelta);

	//LIWCore::s_ins.m_fiberThreadPool.DecreaseSyncCounter(IDX_SYNCCOUNTER_STANDARDSYS_UPDATE);
}

/// <summary>
/// Envoke the RenderUpdate of a standard system. 
/// </summary>
/// <param name=""> StandardSystemData handle(frame) </param>
void FT_SystemRenderUpdate(LIW_FIBER_RUNNER_PARAM) {
	using namespace LIW;

	//auto ptrStdSysData = LIWPointer<StandardSystemData, LIWMem_Frame>((liw_hdl_type)param);
	//auto ptrFrameData = ptrStdSysData->m_hdlFrameData;
	//LIWStandardSystem* const ptrSys = ptrStdSysData->m_system;
	//liw_delete_frame<StandardSystemData>(ptrStdSysData);

	//ptrSys->RenderUpdate(ptrFrameData->m_timeDelta);

	//LIWCore::s_ins.m_fiberThreadPool.DecreaseSyncCounter(IDX_SYNCCOUNTER_STANDARDSYS_RENDERUPDATE);
}