#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "LIWEditorTasks.h"

#include "TestGlobal.h"

void LIW::Editor::LIW_FT_EDTR_UIDrawBeg::Execute(LIWFiberWorkerPointer thisFiber)
{
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
	auto ptrTT_ImguiDrawBeg = new LIW_TT_ImguiDrawBeg();
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_ImguiDrawBeg);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, thisFiber);

	auto ptrFT_EdtrUIDraw = liw_new_def<LIW_FT_EDTR_UIDraw>();
	ptrFT_EdtrUIDraw->ptrFrameData = ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(ptrFT_EdtrUIDraw);
}

void LIW::Editor::LIW_FT_EDTR_UIDraw::Execute(LIWFiberWorkerPointer thisFiber)
{
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
	auto ptrTT_TestDrawUI = new LIWTEST_TT_TestDrawUI();
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_TestDrawUI);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, thisFiber);

	auto ptrFT_EdtrUIDrawEnd = liw_new_def<LIW_FT_EDTR_UIDrawEnd>();
	ptrFT_EdtrUIDrawEnd->ptrFrameData = ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(ptrFT_EdtrUIDrawEnd);
}

void LIW::Editor::LIW_FT_EDTR_UIDrawEnd::Execute(LIWFiberWorkerPointer thisFiber)
{
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
	auto ptrTT_ImguiDrawEndAndPresent = new LIW_TT_ImguiDrawEndAndPresent();
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_ImguiDrawEndAndPresent);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, thisFiber);

	auto ptrFT_FrameEnd = liw_new_def<LIW_FT_FrameEnd>();
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

	if (TestGlobal::cam != liw_c_nullobjhdl) {
		auto& trans = LIW_ECS_GetComponent(LIWComponent_Transform, TestGlobal::cam);
		glm::vec3 rot = Maths::QuatToEulerXZY(trans.m_rotation);
		ImGui::Text("Cam Angles: %.5f, %.5f, %.5f", Maths::RadToDeg(rot.x), Maths::RadToDeg(rot.y), Maths::RadToDeg(rot.z));
		ImGui::Text("Cam Pos: %.5f, %.5f, %.5f", trans.m_location.x , trans.m_location.y, trans.m_location.z);
	}

	ImGuiIO& io = ImGui::GetIO();

	ImGui::Text("WantCaptureMouse: %d", io.WantCaptureMouse);
	ImGui::Text("WantCaptureMouseUnlessPopupClose: %d", io.WantCaptureMouseUnlessPopupClose);
	ImGui::Text("WantCaptureKeyboard: %d", io.WantCaptureKeyboard);
	ImGui::Text("WantTextInput: %d", io.WantTextInput);
	ImGui::Text("WantSetMousePos: %d", io.WantSetMousePos);
	ImGui::Text("NavActive: %d, NavVisible: %d", io.NavActive, io.NavVisible);

	ImGui::End();
	LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
}
