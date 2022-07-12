#pragma once
#include "LIWObject.h"
#include<..\Output\TextOutputDevice.h>

namespace LIW {
	class LIWDebug :
		public LIWObject 
	{
	public:
		LIWDebug(App::TextOutputDevice* textOutput):
			m_textOutput(textOutput){}
		~LIWDebug() {
			if (m_textOutput) {
				delete m_textOutput;
			}
		}

		inline void Print(std::string str) { m_textOutput->Print(str); }
		inline void PrintLine(std::string str) { m_textOutput->PrintLine(str); }
		void DebugPrint(std::string str) {
			PrintLine("[DEBUG INFO]" + str);
		}
		void ErrorPrint(std::string str) {
			PrintLine("[ERROR INFO]" + str);
		}

	protected:
		App::TextOutputDevice* m_textOutput;
	};
}