#pragma once

#include "Memory/LIWMemory.h"
#include "LIWFrame.h"
#include "Threading/LIWFiberTask.h"
#include "Threading/LIWFiberExecutor.h"
#include "Threading/LIWThreadWorkerTask.h"
#include "Threading/LIWMainThreadExecutor.h"

namespace LIW {
	/// <summary>
	/// Update window
	/// </summary>
	class LIW_TT_WindowUpdate final :
		public LIWThreadWorkerTask
	{
	public:
		void Execute() override;
	public:
		LIWPointer<LIWFrameData, LIWMem_Frame> m_ptrFrameData;
	};

	/// <summary>
	/// Poll OS event
	/// </summary>
	class LIW_TT_EventPoll final :
		public LIWThreadWorkerTask
	{
	public:
		void Execute() override;
	};

	/// <summary>
	/// Swap buffer
	/// </summary>
	class LIW_TT_SwapBuffer final :
		public LIWThreadWorkerTask
	{
	public:
		void Execute() override;
	};
}
