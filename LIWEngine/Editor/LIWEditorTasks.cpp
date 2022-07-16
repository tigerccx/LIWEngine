#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "LIWEditorTasks.h"

void LIW::Editor::LIW_FT_EDTR_UIDrawBeg::Execute(LIWFiberWorker* thisFiber)
{
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
	auto ptrTT_ImguiDrawBeg = new LIW_TT_ImguiDrawBeg();
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_ImguiDrawBeg);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, thisFiber);

	auto ptrFT_EdtrUIDraw = new LIW_FT_EDTR_UIDraw();
	ptrFT_EdtrUIDraw->ptrFrameData = ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(ptrFT_EdtrUIDraw);
}

void LIW::Editor::LIW_FT_EDTR_UIDraw::Execute(LIWFiberWorker* thisFiber)
{
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
	auto ptrTT_TestDrawUI = new LIWTEST_TT_TestDrawUI();
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_TestDrawUI);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, thisFiber);

	auto ptrFT_EdtrUIDrawEnd = new LIW_FT_EDTR_UIDrawEnd();
	ptrFT_EdtrUIDrawEnd->ptrFrameData = ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(ptrFT_EdtrUIDrawEnd);
}

void LIW::Editor::LIW_FT_EDTR_UIDrawEnd::Execute(LIWFiberWorker* thisFiber)
{
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
	auto ptrTT_ImguiDrawEndAndPresent = new LIW_TT_ImguiDrawEndAndPresent();
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_ImguiDrawEndAndPresent);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, thisFiber);

	auto ptrFT_FrameEnd = new LIW_FT_FrameEnd();
	ptrFT_FrameEnd->ptrFrameData = ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(ptrFT_FrameEnd);
}

void LIW::Editor::LIW_TT_ImguiDrawBeg::Execute()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
}

void LIW::Editor::LIW_TT_ImguiDrawEndAndPresent::Execute()
{
	// Prerender ImGUI
	ImGui::Render();
	// Render UI
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
}

void LIW::Editor::LIWTEST_TT_TestDrawUI::Execute()
{
	ImGui::Begin("Test");
	ImGui::End();
	LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
}
