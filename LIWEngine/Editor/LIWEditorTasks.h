#pragma once

#include "Memory/LIWMemory.h"
#include "LIWFrame.h"
#include "Fiber/LIWFiberTask.h"
#include "Fiber/LIWFiberExecutor.h"
#include "Fiber/LIWThreadWorkerTask.h"
#include "Fiber/LIWMainThreadExecutor.h"

namespace LIW {
	namespace Editor {
		/// <summary>
		/// Begin drawing editor UI
		/// </summary>
		class LIW_FT_EDTR_UIDrawBeg final :
			public LIWFiberTask
		{
		public:
			void Execute(LIWFiberWorker* thisFiber) override;
		public:
			LIWPointer<LIWFrameData, LIWMem_Frame> ptrFrameData;
		};

		/// <summary>
		/// Draw editor UI
		/// </summary>
		class LIW_FT_EDTR_UIDraw final :
			public LIWFiberTask
		{
		public:
			void Execute(LIWFiberWorker* thisFiber) override;
		public:
			LIWPointer<LIWFrameData, LIWMem_Frame> ptrFrameData;
		};

		/// <summary>
		/// End drawing editor UI
		/// </summary>
		class LIW_FT_EDTR_UIDrawEnd final :
			public LIWFiberTask
		{
		public:
			void Execute(LIWFiberWorker* thisFiber) override;
		public:
			LIWPointer<LIWFrameData, LIWMem_Frame> ptrFrameData;
		};


		/// <summary>
		/// Begin imgui drawing
		/// </summary>
		class LIW_TT_ImguiDrawBeg final :
			public LIWThreadWorkerTask
		{
		public:
			void Execute() override;
		};

		/// <summary>
		/// End imgui drawing and present
		/// </summary>
		class LIW_TT_ImguiDrawEndAndPresent final :
			public LIWThreadWorkerTask
		{
		public:
			void Execute() override;
		};


		class LIWTEST_TT_TestDrawUI final :
			public LIWThreadWorkerTask
		{
		public:
			void Execute() override;
		};
	}
}

