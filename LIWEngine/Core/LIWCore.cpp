#include "LIWCore.h"

LIW::LIWCore LIW::LIWCore::s_ins;

//#include "Editor/LIWEditorUIHelper.h"

void TT_TestUIDraw(LIW_THREADWORKER_RUNNER_PARAM) {
	
	ImGui::Begin("Entities");
	//auto entities = LIWEntityManager::instance.GetEntityList();
	//for (auto& itr : entities) {
	//	LIW::Editor::LIWEditorUIHelper::ShowEntity(itr);
	//}
	ImGui::End();

	LIW::LIWCore::s_ins.m_fiberThreadPool.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
}