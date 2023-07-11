#pragma once

#include "LIWDebug.h"
#include "Threading/LIWThreadWorkerTask.h"

namespace LIW {
	/// <summary>
	/// Print debug line
	/// </summary>
	class LIW_TT_DebugPrint final :
		public LIWThreadWorkerTask
	{
	public:
		void Execute() override;
	public:
		LIWPointer<LIWDebug, LIWMem_Static> m_ptrDebug;
		std::string m_line;
	};

	/// <summary>
	/// Print error line
	/// </summary>
	class LIW_TT_ErrorPrint final :
		public LIWThreadWorkerTask
	{
	public:
		void Execute() override;
	public:
		LIWPointer<LIWDebug, LIWMem_Static> m_ptrDebug;
		std::string m_line;
	};
}