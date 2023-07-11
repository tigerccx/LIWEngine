#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "LIWEditorTasks.h"

#ifdef LIW_ENABLE_EDITOR

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
	ptrTT_TestDrawUI->ptrFrameData = ptrFrameData;
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

	ImGui::Text("Framerate: %.2f", 1.0f / ptrFrameData->m_timeDelta);


	ImGui::Separator();

	if (TestGlobal::cam != liw_c_nullobjhdl) {
		auto& trans = LIW_ECS_GetComponent(LIWComponent_Transform, TestGlobal::cam);
		glm::vec3 rot = Maths::QuatToEulerXZY(trans.m_rotation);
		ImGui::Text("Cam Angles: %.5f, %.5f, %.5f", Maths::RadToDeg(rot.x), Maths::RadToDeg(rot.y), Maths::RadToDeg(rot.z));
		ImGui::Text("Cam Pos: %.5f, %.5f, %.5f", trans.m_position.x , trans.m_position.y, trans.m_position.z);
	}

	ImGui::Separator();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Separator();

	ImGuiIO& io = ImGui::GetIO();

	ImGui::Text("WantCaptureMouse: %d", io.WantCaptureMouse);
	ImGui::Text("WantCaptureMouseUnlessPopupClose: %d", io.WantCaptureMouseUnlessPopupClose);
	ImGui::Text("WantCaptureKeyboard: %d", io.WantCaptureKeyboard);
	ImGui::Text("WantTextInput: %d", io.WantTextInput);
	ImGui::Text("WantSetMousePos: %d", io.WantSetMousePos);
	ImGui::Text("NavActive: %d, NavVisible: %d", io.NavActive, io.NavVisible);

	ImGui::End();

	DrawSceneGraphUI();
	DrawMaterialManagerUI();

	LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
}

void LIW::Editor::DrawSceneGraphUI()
{
	ImGui::Begin("Scene");

	auto& entityManager = LIW_ECS_GetEntityManager();
	auto& entities = entityManager.GetOccupiedEntities();

	const size_t entityCount = entities.size();

	LIWDArray<std::string, LIWMem_Default> items{ entityCount };
	int i = 0;
	for (auto itr = entities.begin(); itr!=entities.end(); itr++, i++) {
		items.push_back(std::string(32, '\0'));
		items[i].resize(32);
		sprintf_s((char*)items[i].data(), 32, "Entity %u", *itr);
	}

	static liw_objhdl_type selectedEntity = liw_c_nullobjhdl;
	if (ImGui::BeginListBox("Scene Entities"))
	{
		int n = 0;
		for (auto itr = entities.begin(); itr != entities.end(); itr++, n++)
		{
			const liw_objhdl_type hdlEntity = *itr;
			const bool is_selected = (hdlEntity == selectedEntity);
			if (ImGui::Selectable(items[n].c_str(), is_selected))
				selectedEntity = hdlEntity;

			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}

	if (selectedEntity != liw_c_nullobjhdl) {
		ImGui::Separator();
		liw_objhdl_type hdlTrans = LIW_ECS_GetComponentFromEntity(LIWComponent_Transform, selectedEntity);
		auto& trans = LIW_ECS_GetComponent(LIWComponent_Transform, hdlTrans);
		trans.EditorDrawUI();
	}

	ImGui::End();
}

void LIW::Editor::DrawMaterialManagerUI()
{
	ImGui::Begin("Materials");

	auto& assetManager = *LIWGlobal::GetAssetManager();
	auto& materials = assetManager.GetMaterials();

	LIWDArray<const char*, LIWMem_Default> items;

	const int countMaterial = materials.size();
	for (auto itr = materials.begin(); itr != materials.end(); itr++) {
		items.push_back(itr->first.c_str());
	}

	static LIWPointer<LIWMaterial, LIWMem_Default> selectedMaterial = liw_c_nullobjhdl;
	if (ImGui::BeginListBox("Materials"))
	{
		int n = 0;
		for (auto itr = materials.begin(); itr != materials.end(); itr++)
		{
			const bool is_selected = (itr->second == selectedMaterial);
			if (ImGui::Selectable(items[n], is_selected))
				selectedMaterial = itr->second;

			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
			n++;
		}
		ImGui::EndListBox();
	}

	if (selectedMaterial != liw_c_nullobjhdl) {
		ImGui::Separator();
		auto& material = *selectedMaterial;
		material.EditorDrawUI();
	}

	ImGui::End();
}

#endif //LIW_ENABLE_EDITOR