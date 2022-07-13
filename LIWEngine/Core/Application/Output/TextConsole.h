#pragma once
#include <iostream>
#include "TextOutputDevice.h"

namespace LIW {
	namespace App {
		class TextConsole :
			public TextOutputDevice 
		{
		public:
			virtual void Print(std::string str) override;
			virtual void PrintLine(std::string str) override;
		};
	}
}