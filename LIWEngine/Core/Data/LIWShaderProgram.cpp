#include "LIWShaderProgram.h"

namespace LIW {
	void LIWShaderProgram::CreateShader(const std::vector<LIWShader>& shaders)
	{
		printf("Creating Program from: \n");
		for (int i = 0; i < shaders.size(); i++) {
			if (!shaders[i].IsValid())
				throw std::runtime_error("shader not valid. ");
			printf("  %s\n", shaders[i].GetName());
		}

		// Create program and attach shaders
		m_handleShaderProgram = glCreateProgram();
		// Attach shaders
		int idxShader = 0;
		for (idxShader = 0; idxShader < shaders.size(); idxShader++) {
			const uint32_t handleShader = shaders[idxShader].GetRawHandle();
			glAttachShader(m_handleShaderProgram, handleShader);
			//m_handleShaders[idxShader] = m_handleShaderProgram;
		}
		if (idxShader < 5)
			//m_handleShaders[idxShader] = sc_invalidHandle;
		// Link
		glLinkProgram(m_handleShaderProgram);
		int programState = 0;
		glGetProgramiv(m_handleShaderProgram, GL_LINK_STATUS, &programState);
		// Print Debug
		if (!programState) {
			printf("Linking failed!\n");
			PrintLinkLog();
			throw std::runtime_error("shader program link failed. ");
		}
		else {
			printf("Linking success!\n");
		}
		for (idxShader = 0; idxShader < shaders.size(); idxShader++) {
			const uint32_t handleShader = shaders[idxShader].GetRawHandle();
			glDetachShader(m_handleShaderProgram, handleShader);
		}
	}
	void LIWShaderProgram::DestroyShader()
	{
		if (!IsValid())
			throw std::runtime_error("shader program not created. ");
		//for (int i = 0; i < 5; i++) {
		//	if (m_handleShaders[i] != sc_invalidHandle)
		//		glDetachShader(m_handleShaderProgram, m_handleShaders[i]);
		//	else
		//		break;
		//}
		glDeleteProgram(m_handleShaderProgram);
		//m_handleShaderProgram = m_handleShaders[0] = sc_invalidHandle;
	}
	void LIWShaderProgram::PrintLinkLog()
	{
		int logLength = 0;
		glGetProgramiv(m_handleShaderProgram, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength) {
			char* tempData = new char[logLength];
			glGetProgramInfoLog(m_handleShaderProgram, logLength, NULL, tempData);
			printf("Link Log:\n %s\n", tempData);
			delete[] tempData;
		}
	}
}