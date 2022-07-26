#pragma once
#include <fstream>
#include <vector>
#include <string>

//TODO: Use LIWMemory
//TODO: Implement LIWString and use that

namespace LIW {
	class LIWFile {
	public:
		static std::vector<char> ReadFile(const char* filePath, bool isBinary = false);
		static std::vector<char> ReadTextFileWithInclude(const char* filePath);
		static void PrintFileContent(const std::vector<char>& content);
	
		static bool GetDirectory(const std::string& filePath, std::string& directoryOut);
		static bool GetFilename(const std::string& filePath, std::string& fileNameOut);
	private:
		static bool GetInclude(const std::string& line, std::string& includeFileOut);
	};
}