#include "LIWFile.h"

std::vector<char> LIW::LIWFile::ReadFile(const char* filePath, bool isBinary)
{
	auto fileMode = isBinary ? std::ios::binary : 0;
	std::ifstream file(filePath, std::ios::ate | fileMode);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}
