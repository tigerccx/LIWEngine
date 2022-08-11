#include "LIWMaterial.h"
#include "LIWGlobal.h"

namespace LIW {
	void LIWMaterial::ClearParams()
	{
		m_paramsInt.clear();
		m_paramsFloat.clear();
		m_paramsIVec4.clear();
		m_paramsFVec4.clear();
		m_paramsTex2D.clear();
	}
	void LIWMaterial::BindData(uint32_t texUnitBegin)
	{
		auto& assetManager = *LIWGlobal::GetAssetManager();
		auto& shaderProgram = assetManager.GetShaderProgram(m_handleShaderProgram);
		uint32_t rawHandleShaderProgram = shaderProgram.GetRawHandle();
		
		for (int i = 0; i < m_paramsInt.get_size(); i++) {
			glUniform1i(glGetUniformLocation(rawHandleShaderProgram, m_paramsInt[i].m_name), 
				m_paramsInt[i].m_val);
		}
		for (int i = 0; i < m_paramsFloat.get_size(); i++) {
			glUniform1f(glGetUniformLocation(rawHandleShaderProgram, m_paramsFloat[i].m_name),
				m_paramsFloat[i].m_val);
		}
		for (int i = 0; i < m_paramsIVec4.get_size(); i++) {
			glUniform4i(glGetUniformLocation(rawHandleShaderProgram, m_paramsIVec4[i].m_name),
				m_paramsIVec4[i].m_val.x, 
				m_paramsIVec4[i].m_val.y, 
				m_paramsIVec4[i].m_val.z, 
				m_paramsIVec4[i].m_val.w);
		}
		for (int i = 0; i < m_paramsFVec4.get_size(); i++) {
			glUniform4f(glGetUniformLocation(rawHandleShaderProgram, m_paramsFVec4[i].m_name),
				m_paramsFVec4[i].m_val.x,
				m_paramsFVec4[i].m_val.y,
				m_paramsFVec4[i].m_val.z,
				m_paramsFVec4[i].m_val.w);
		}
		uint32_t texUnit = texUnitBegin;
		for (int i = 0; i < m_paramsTex2D.get_size(); i++) {
			auto& tex2D = assetManager.GetTexture2D(m_paramsTex2D[i].m_val);
			if (!tex2D.IsValid())
				throw std::runtime_error("Texture not valid. \n");
			LIWTexture::Bind2DTexture(tex2D.GetRawHandle(), rawHandleShaderProgram, m_paramsTex2D[i].m_name, texUnit);
			texUnit++;
		}
	}
	void LIWMaterial::UnbindData()
	{
		auto& assetManager = *LIWGlobal::GetAssetManager();
		uint32_t texUnit = 0;
		for (int i = 0; i < m_paramsTex2D.get_size(); i++) {
			auto& tex2D = assetManager.GetTexture2D(m_paramsTex2D[i].m_val);
			LIWTexture::Unbind2DTexture(texUnit);
			texUnit++;
		}
	}
}