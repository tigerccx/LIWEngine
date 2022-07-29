#pragma once

#include "Maths/LIWMaths.h"
#include "Memory/LIWMemory.h"
#include "Containers/LIWDArray.h"
#include "LIWTexture.h"
#include "LIWShaderProgram.h"

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
	typedef LIWMaterialParam<uint32_t> LIWMaterialParam_Tex2D;

	class LIWMaterial {
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
		inline void AddParam_Tex2D(const char* name, uint32_t val) 
		{
			LIWMaterialParam_Tex2D param{ "", val };
			strcpy_s(param.m_name, LIW_MATERIAL_PARAM_MAX_NAME_LEN, name);
			m_paramsTex2D.push_back(param);
		}

		void ClearParams();

		void BindData();
		void UnbindData();

	public:
		liw_objhdl_type m_handleShaderProgram{ liw_c_nullobjhdl };
		LIWDArray<LIWMaterialParam_Int>		m_paramsInt{ 0 };
		LIWDArray<LIWMaterialParam_Float>	m_paramsFloat{ 0 };
		LIWDArray<LIWMaterialParam_IVec4>	m_paramsIVec4{ 0 };
		LIWDArray<LIWMaterialParam_FVec4>	m_paramsFVec4{ 0 };
		LIWDArray<LIWMaterialParam_Tex2D>	m_paramsTex2D{ 0 };
	};
}