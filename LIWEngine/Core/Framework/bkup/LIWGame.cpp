#include "LIWGame.h"

LIW::LIWGame* LIW::LIWGame::instance = nullptr;

int LIW::LIWGame::Initialise()
{
	return 0;
}

int LIW::LIWGame::UpdateMainLoop()
{
	m_timer->Tick();

	float dt = m_timer->GetTimeDeltaSeconds();

	/*PreFrame*/
	for (auto itr : m_standardSystems) {
		itr->PreFrame(dt);
	}

	/*Update*/
	for (auto itr : m_standardSystems) {
		itr->Update(dt);
	}

	/*FixedUpdate*/
	m_dtFixedAccum += dt;
	LIWGameTimer t;
	t.GetTimeDeltaSeconds();
	while (m_dtFixedAccum >= m_realDT) {
		for (auto itr : m_standardSystems) {
			itr->FixedUpdate(dt);
		}
		m_dtFixedAccum -= m_realDT;
	}
	t.Tick();
	float updateTime = t.GetTimeDeltaSeconds();
	//Uh oh, physics is taking too long...
	if (updateTime > m_realDT) {
		m_realHZ /= 2;
		m_realDT *= 2;
		std::string str = "Dropping iteration count due to long physics time...(now " + std::to_string(m_realHZ) + ")\n";
		m_debug->DebugPrint(str);
	}
	else if (dt * 2 < m_realDT) { //we have plenty of room to increase iteration count!
		int temp = m_realHZ;
		m_realHZ *= 2;
		m_realDT /= 2;

		if (m_realHZ > m_idealHZ) {
			m_realHZ = m_idealHZ;
			m_realDT = m_idealDT;
		}
		if (temp != m_realHZ) {
			std::string str = "Raising iteration count due to short physics time...(now " + std::to_string(m_realHZ) + ")\n";
			m_debug->DebugPrint(str);
		}
	}

	/*PreRender*/
	for (auto itr : m_standardSystems) {
		itr->PreRenderUpdate(dt);
	}

	/*Render*/
	for (auto itr : m_standardSystems) {
		itr->RenderUpdate(dt);
	}

	/*PostRenderUpdate*/
	for (auto itr : m_standardSystems) {
		itr->PostRenderUpdate(dt);
	}

	/*PostFrame*/
	for (auto itr : m_standardSystems) {
		itr->PostFrame(dt);
	}

	return 0;
}

int LIW::LIWGame::CleanUp()
{
	for (auto itr : m_standardSystems) {
		if (itr)
			delete itr;
	}
	m_standardSystems.clear();
	return 0;
}



#include "LIWCore.h"
#include "LIWStandardSystem.h"

void FT_GameUpdate(LIW_FIBER_RUNNER_PARAM)
{
	using namespace LIW;

	LIWPointer<GameData, LIWMem_Frame> ptrData((liw_hdl_type)param);
	auto ptrFrameData = ptrData->m_hdlFrameData;
	LIWGame* const ptrGame = ptrData->m_game;
	liw_delete_frame<GameData>((liw_hdl_type)param);

	auto& systems = ptrGame->m_standardSystems;
	const int systemCount = (int)systems.size();

	LIWCore::s_ins.m_fiberThreadPool.IncreaseSyncCounter(IDX_SYNCCOUNTER_STANDARDSYS_UPDATE, systemCount);

	for (int i = 0; i < systems.size(); i++) {
		auto ptrStdSysData = liw_new_frame<StandardSystemData>(StandardSystemData{ ptrFrameData, systems[i] });
		LIWCore::s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ FT_SystemUpdate, (void*)ptrStdSysData.get_handle() });
	}
	LIWCore::s_ins.m_fiberThreadPool.WaitOnSyncCounter(IDX_SYNCCOUNTER_STANDARDSYS_UPDATE, thisFiber);


	LIWCore::s_ins.m_fiberThreadPool.IncreaseSyncCounter(IDX_SYNCCOUNTER_STANDARDSYS_RENDERUPDATE, systemCount);
	for (int i = 0; i < systems.size(); i++) {
		auto ptrStdSysData = liw_new_frame<StandardSystemData>(StandardSystemData{ ptrFrameData, systems[i] });
		LIWCore::s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ FT_SystemRenderUpdate, (void*)ptrStdSysData.get_handle() });
	}
	LIWCore::s_ins.m_fiberThreadPool.WaitOnSyncCounter(IDX_SYNCCOUNTER_STANDARDSYS_RENDERUPDATE, thisFiber);

}