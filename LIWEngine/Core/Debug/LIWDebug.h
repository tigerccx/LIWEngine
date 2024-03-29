#pragma once
#include "LIWObject.h"
#include "Memory/LIWMemory.h"
#include "Application/Output/TextOutputDevice.h"

namespace LIW {
	class LIWDebug :
		public LIWObject 
	{
	public:
		LIWDebug(LIWPointer<App::TextOutputDevice, LIWMem_Static> textOutput):
			m_textOutput(textOutput){

		}
		~LIWDebug() {
			//if (m_textOutput) {
			//	delete m_textOutput;
			//}
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
		LIWPointer<App::TextOutputDevice, LIWMem_Static> m_textOutput{ liw_c_nullhdl };
	};
}