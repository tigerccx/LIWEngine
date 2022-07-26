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

std::vector<char> LIW::LIWFile::ReadTextFileWithInclude(const char* filePath)
{
	std::ifstream file(filePath, std::ios::in);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	std::string directory;
	GetDirectory(filePath, directory);

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);

	std::string line, includeFile;
	while (!file.eof()) {
		getline(file, line);
		if (GetInclude(line, includeFile)) {
			includeFile = directory + "/" + includeFile;
			auto includeContent = ReadTextFileWithInclude(includeFile.c_str());
			buffer.insert(buffer.end(), includeContent.begin(), includeContent.end());
			includeFile.clear();
		}
		else {
			line += "\n";
			buffer.insert(buffer.end(), line.begin(), line.end());
		}
	}

	file.close();

	return buffer;
}

void LIW::LIWFile::PrintFileContent(const std::vector<char>& content)
{
	int lineNum = 1;
	for (int i = 0; i < content.size();) {
		int beg = i;
		int end;
		for (end = i; content[end] != '\n'; end++) {}
		printf("%4d: %.*s\n", lineNum, end - beg, &content[beg]); // Only support up to 9999 lines
		i = end + 1;
		lineNum++;
	}
}

bool LIW::LIWFile::GetDirectory(const std::string& filePath, std::string& directoryOut)
{
	//NOTE: only recognize / as separator
	const size_t last_slash_idx = filePath.find_last_of("/"); 
	if (std::string::npos != last_slash_idx)
	{
		directoryOut = filePath.substr(0, last_slash_idx);
		return true;
	}
	else
	{
		return false;
	}
}

bool LIW::LIWFile::GetFilename(const std::string& filePath, std::string& fileNameOut)
{
	//NOTE: only recognize / as separator
	const size_t last_slash_idx = filePath.find_last_of("/");
	if (std::string::npos != last_slash_idx)
	{
		fileNameOut = filePath.substr(last_slash_idx+1);
		return true;
	}
	else
	{
		fileNameOut = filePath;
		return true;
	}
}

bool LIW::LIWFile::GetInclude(const std::string& line, std::string& includeFileOut)
{
	if (line.substr(0, 8) != "#include")
		return false;
	else {
		size_t quoteFront = line.find_first_of('\"');
		if (quoteFront >= line.size()) {
			return false;
		}
		size_t quoteBack = line.find_first_of('\"', quoteFront + 1);
		if (quoteBack >= line.size()) {
			return false;
		}
		includeFileOut = line.substr(quoteFront + 1, quoteBack - quoteFront - 1);
		return true;
	}
}
