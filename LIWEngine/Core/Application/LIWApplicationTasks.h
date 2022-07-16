#pragma once

#include "Memory/LIWMemory.h"
#include "LIWFrame.h"
#include "Fiber/LIWFiberTask.h"
#include "Fiber/LIWFiberExecutor.h"
#include "Fiber/LIWThreadWorkerTask.h"
#include "Fiber/LIWMainThreadExecutor.h"

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
