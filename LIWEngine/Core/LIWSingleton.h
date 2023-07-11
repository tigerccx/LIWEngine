#pragma once

#include <cassert>

namespace LIW
{
	template<class T>
	class LIWSingleton
	{
	public:
		LIWSingleton()
		{
			assert(!s_instance); // You can only have 1 instance of a singleton! 
			s_instance = this;
		}

		~LIWSingleton()
		{
			s_instance = nullptr;
		}

		bool IsValid() { return s_instance != nullptr; }
		T& Get() { return *s_instance; }

	private:
		static T* s_instance;
	};
}
