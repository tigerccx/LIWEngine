#pragma once
#include "LIWConfig.h"

#include "Maths/LIWMaths.h"
#include "Memory/LIWMemory.h"
#include "Containers/LIWDArray.h"
#include "LIWTexture.h"
#include "LIWShaderProgram.h"

#ifdef LIW_ENABLE_EDITOR
#include "Editor/LIWIEditorDraw.h"
#endif //LIW_ENABLE_EDITOR

#define LIW_MATERIAL_PARAM_MAX_NAME_LEN 32

namespace LIW {
	enum LIWMaterialParamType {
		LIWMaterialParamType_Int,
		LIWMaterialParamType_Float,
		LIWMaterialParamType_IVec4,
		LIWMaterialParamType_FVec4,
		LIWMaterialParamType_Tex2D
	};
	template<class T>
	struct LIWMaterialParam {
		char m_name[LIW_MATERIAL_PARAM_MAX_NAME_LEN]; //NOTE: Only allow 31 chars. 
		T m_val;
	};
	typedef LIWMaterialParam<int32_t> LIWMaterialParam_Int;
	typedef LIWMaterialParam<float> LIWMaterialParam_Float;
	typedef LIWMaterialParam<glm::ivec4> LIWMaterialParam_IVec4;
	typedef LIWMaterialParam<glm::vec4> LIWMaterialParam_FVec4;
	typedef LIWMaterialParam<LIWPointer<LIWTexture2D, LIWMem_Default>> LIWMaterialParam_Tex2D;

	class LIWMaterial
#ifdef LIW_ENABLE_EDITOR
		: public Editor::LIWIEditorDraw
#endif //LIW_ENABLE_EDITOR
	{
	public:
		//inline void SetShaderProgram(liw_objhdl_type handleProgram) {
		//	m_handleShaderProgram = handleProgram;
		//}

		inline void AddParam_Int(const char* name, int val) {
			LIWMaterialParam_Int param{ "", val };
			strcpy_s(param.m_name, LIW_MATERIAL_PARAM_MAX_NAME_LEN, name);
			m_paramsInt.push_back(param);
		}
		inline void AddParam_Float(const char* name, float val) {
			LIWMaterialParam_Float param{ "", val };
			strcpy_s(param.m_name, LIW_MATERIAL_PARAM_MAX_NAME_LEN, name);
			m_paramsFloat.push_back(param);
		}
		inline void AddParam_IVec4(const char* name, glm::ivec4 val) {
			LIWMaterialParam_IVec4 param{ "", val };
			strcpy_s(param.m_name, LIW_MATERIAL_PARAM_MAX_NAME_LEN, name);
			m_paramsIVec4.push_back(param);
		}
		inline void AddParam_FVec4(const char* name, glm::vec4 val) {
			LIWMaterialParam_FVec4 param{ "", val };
			strcpy_s(param.m_name, LIW_MATERIAL_PARAM_MAX_NAME_LEN, name);
			m_paramsFVec4.push_back(param);
		}
		inline void AddParam_Tex2D(const char* name, LIWPointer<LIWTexture2D, LIWMem_Default> val)
		{
			LIWMaterialParam_Tex2D param{ "", val };
			strcpy_s(param.m_name, LIW_MATERIAL_PARAM_MAX_NAME_LEN, name);
			m_paramsTex2D.push_back(param);
		}

		void ClearParams();

		void BindData(uint32_t texUnitBegin = 0);
		void UnbindData();

	public:
		LIWPointer<LIWShaderProgram, LIWMem_Default> m_handleShaderProgram{ liw_c_nullobjhdl };
		LIWDArray<LIWMaterialParam_Int>		m_paramsInt{ 0 };
		LIWDArray<LIWMaterialParam_Float>	m_paramsFloat{ 0 };
		LIWDArray<LIWMaterialParam_IVec4>	m_paramsIVec4{ 0 };
		LIWDArray<LIWMaterialParam_FVec4>	m_paramsFVec4{ 0 };
		LIWDArray<LIWMaterialParam_Tex2D>	m_paramsTex2D{ 0 };

#ifdef LIW_ENABLE_EDITOR
	public:
		inline void EditorDrawUI() override {
			for (int i = 0; i < m_paramsInt.get_size(); i++) {
				ImGui::DragInt(m_paramsInt[i].m_name, &m_paramsInt[i].m_val);
			}
			for (int i = 0; i < m_paramsFloat.get_size(); i++) {
				ImGui::DragFloat(m_paramsFloat[i].m_name, &m_paramsFloat[i].m_val);
			}
			for (int i = 0; i < m_paramsIVec4.get_size(); i++) {
				ImGui::DragInt4(m_paramsIVec4[i].m_name, glm::value_ptr(m_paramsIVec4[i].m_val));
			}
			for (int i = 0; i < m_paramsFVec4.get_size(); i++) {
				ImGui::DragFloat4(m_paramsFVec4[i].m_name, glm::value_ptr(m_paramsFVec4[i].m_val));
			}
			for (int i = 0; i < m_paramsTex2D.get_size(); i++) {
				ImGui::Text("Texture: %s", m_paramsTex2D[i].m_name);
			}
		}
#endif
	};
}