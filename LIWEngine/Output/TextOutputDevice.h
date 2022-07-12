#pragma once
#include <string>

#include "OutputDevice.h"

namespace LIW {
	namespace App {
		class TextOutputDevice :
			public OutputDevice
		{
		public:
			virtual void Print(std::string str) = 0;
			virtual void PrintLine(std::string str) = 0;
		};
	}
}


