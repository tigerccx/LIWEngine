#pragma once
#include <stdexcept>

class liwexcept_out_of_memory : public std::exception {
public:
	const char* what() const override { return "Out of mem! All memory allocated! Reduce memory consumption!\n"; }
};

class liwexcept_memory_corruption : public std::exception {
public:
	liwexcept_memory_corruption(void* ptr) {
		snprintf(m_info, 50, "Memory corruption at %p\n", ptr);
	}
	const char* what() const override { return m_info; }
private:
	char m_info[50];
};