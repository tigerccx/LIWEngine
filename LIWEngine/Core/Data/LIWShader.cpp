#include "LIWShader.h"

namespace LIW {
	const std::unordered_map<LIWShaderType, GLenum> LIWImageFormat_2_GLShaderType =
	{
		{LIWShaderType_Vertex,		GL_VERTEX_SHADER			},
		{LIWShaderType_Hull,		GL_TESS_CONTROL_SHADER		},
		{LIWShaderType_Domain,		GL_TESS_EVALUATION_SHADER	},
		{LIWShaderType_Geometry,	GL_GEOMETRY_SHADER			},
		{LIWShaderType_Fragment,	GL_FRAGMENT_SHADER			},
		{LIWShaderType_Compute,		GL_COMPUTE_SHADER			}
	};

	void LIWShader::LoadShader(const char* filePath, LIWShaderType shaderType)
	{
		if(IsValid())
			throw std::runtime_error("shader already loaded. ");

		if (shaderType >= LIWShaderType_Max)
			throw std::runtime_error("shader type invalid. ");
		
		std::string fileName;
		LIWFile::GetFilename(filePath, fileName);

		printf("Compiling: %s\n", fileName.c_str());

		auto content = LIWFile::ReadTextFileWithInclude(filePath);
		const char* chars = content.data();
		int len = content.size();

#ifdef LIW_DEBUG_PRINT_SHADER_CONTENT
		int line = 1;
		for (int beg = 0; beg < len;) {
			int end = beg;
			for (; end < len - 1; end++) {
				if (chars[end] == '\n') break;
			}
			end += 1;
			const int lenLine = end - beg;
			printf("%04d %*.*s", line, lenLine, lenLine, chars + beg);
			line++;
			beg = end;
		}
#endif

		m_handleShader = glCreateShader(LIWImageFormat_2_GLShaderType.at(shaderType));
		m_shaderType = shaderType;
		strcpy_s(m_name, fileName.c_str());
		glShaderSource(m_handleShader, 1, &chars, &len);
		glCompileShader(m_handleShader);
		

		int shaderState = 0;
		glGetShaderiv(m_handleShader, GL_COMPILE_STATUS, &shaderState);	//Retrieve compiling state. 

		if (!shaderState) {
			printf("Compiling failed!\n");
			PrintCompileLog();
			throw std::runtime_error("shader compilation failed. ");
		}
		else {
			printf("Compiling success!\n");
		}

		glObjectLabel(GL_SHADER, m_handleShader, -1, filePath);	//Mark label for debug. 
	}
	void LIWShader::UnloadShader()
	{
		if (!IsValid())
			throw std::runtime_error("shader not loaded. ");
		glDeleteShader(m_handleShader);
		m_handleShader = sc_invalidHandle;
		m_shaderType = LIWShaderType_Max;
		m_name[0] = '\0';
	}
	void LIWShader::PrintCompileLog()
	{
		int logLength = 0;
		glGetShaderiv(m_handleShader, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength) {
			char* tempData = new char[logLength];
			glGetShaderInfoLog(m_handleShader, logLength, NULL, tempData);
			printf("Compile Log: \n%s\n", tempData);
			delete[] tempData;
		}
	}
}
