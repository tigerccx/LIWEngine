#pragma once
#include <vector>
#include <map>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Maths/Vector2.h"
#include "Maths/Vector3.h"
#include "Maths/Vector4.h"

#include "Framework/LIWEntity.h"

namespace LIW {
	namespace Editor {
#define LIW_EDITOR_STRING_INPUT_BUFFER_SIZE 100

		struct ImGuiExtendedType {
		public:
			ImGuiExtendedType() {};
			ImGuiExtendedType(ImGuiDataType_ t, int l) :type(t), length(l) { }
			ImGuiDataType_ type{ ImGuiDataType_COUNT };
			int length{ 0 };
		};

		//static std::map<std::string, ImGuiExtendedType> typeNameToImGui;

		class LIWEditorUIHelper {
		public:
			static bool ShowDrag(const std::string& label, const std::string& type, void* ptVal) {
				//if (type == GetTypeName<float>()) {
				//	ImGui::DragFloat(label.c_str(), (float*)ptVal);
				//	return true;
				//}
				//if (type == GetTypeName<Vector2>()) {
				//	ImGui::DragFloat2(label.c_str(), (float*)ptVal);
				//	return true;
				//}
				//if (type == GetTypeName<Vector3>()) {
				//	ImGui::DragFloat3(label.c_str(), (float*)ptVal);
				//	return true;
				//}
				//if (type == GetTypeName<Vector4>()) {
				//	ImGui::DragFloat4(label.c_str(), (float*)ptVal);
				//	return true;
				//}
				//if (type == GetTypeName<int>()) {
				//	ImGui::DragInt(label.c_str(), (int*)ptVal);
				//	return true;
				//}
				return false;
			}

			static bool ShowString(const std::string& label, const std::string& type, void* ptVal) {
				//auto a = GetTypeName<std::string>();
				//if (type == a) {
				//	std::string* ptStr = (std::string*)ptVal;
				//	char text[LIW_EDITOR_STRING_INPUT_BUFFER_SIZE] = "";
				//	size_t sizeCopy = min(ptStr->size(), LIW_EDITOR_STRING_INPUT_BUFFER_SIZE);
				//	auto subString = ptStr->substr(0, sizeCopy);
				//	subString.copy(text, sizeCopy, 0);
				//	ImGui::InputText(label.c_str(), text, LIW_EDITOR_STRING_INPUT_BUFFER_SIZE);
				//	*ptStr = text;
				//	return true;
				//}
				return false;
			}

			static void ShowComponent(LIWComponent* component) {
				//if (ImGui::TreeNode(component->GetName().c_str())) {
				//	LIWReflectAttrBook& attrs = component->GetObjectType()->GetAttrs();
				//	for (auto& attr : attrs) {
				//		void* ptVoid = component->GetObjectType()->GetPtVoid(component, attr.m_name);
				//		if (!ShowDrag(attr.m_name, attr.m_type->GetName(), ptVoid)) {
				//			if (!ShowString(attr.m_name, attr.m_type->GetName(), ptVoid)) {

				//			}
				//		}
				//	}
				//	ImGui::TreePop();
				//}
			}

			static void ShowEntity(LIWEntity* entity) {
				//if (ImGui::TreeNode(entity->GetName().c_str())) {
				//	const std::vector<LIWComponent*>& components = entity->GetComponentList();
				//	for (auto& itr : components) {
				//		ShowComponent(itr);
				//	}
				//	ImGui::TreePop();
				//}
			}

			/*static ImGuiExtendedType TypeNameToImGuiExtendedType(const std::string typeName) {
				auto itr = typeNameToImGui.find(typeName);
				assert(itr != typeNameToImGui.end());
				return itr->second;
			}*/

			static void TestShow() {
				//float b = 0.0f;
				//if (ImGui::TreeNode("Tree00")) {
				//	if (ImGui::TreeNode("Tree10")) {
				//		ImGui::DragFloat("a0", &b);
				//		ImGui::TreePop();
				//	}
				//	if (ImGui::TreeNode("Tree11")) {
				//		ImGui::DragFloat("a1", &b);
				//		ImGui::TreePop();
				//	}
				//	ImGui::TreePop();
				//}
			}
		protected:
			LIWEditorUIHelper() {
				/*typeNameToImGui[GetTypeName<int>()] = ImGuiExtendedType(ImGuiDataType_S32, 1);
				typeNameToImGui[GetTypeName<float>()] = ImGuiExtendedType(ImGuiDataType_Float, 1);*/
			}
		};
	}
}