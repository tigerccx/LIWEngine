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

		// Create program
		m_handleShaderProgram = glCreateProgram();
		// Attach shaders
		for (int i = 0; i < shaders.size(); i++) {
			const uint32_t handleShader = shaders[i].GetRawHandle();
			glAttachShader(m_handleShaderProgram, handleShader);
			m_handleShaders.push_back(m_handleShaderProgram);
		}
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
	}
	void LIWShaderProgram::DestroyShader()
	{
		if (!IsValid())
			throw std::runtime_error("shader program not created. ");
		for (int i = 0; i < m_handleShaders.get_size(); i++) {
			glDetachShader(m_handleShaderProgram, m_handleShaders[i]);
		}
		glDeleteProgram(m_handleShaderProgram);
		m_handleShaderProgram = sc_invalidHandle;
		m_handleShaders.clear();
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