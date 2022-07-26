#pragma once
#include <fstream>
#include <vector>

//TODO: Use LIWMemory

namespace LIW {
	class LIWFile {
	public:
		static std::vector<char> ReadFile(const char* file, bool isBinary = false);
	};
}