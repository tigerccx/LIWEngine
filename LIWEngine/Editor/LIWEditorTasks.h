#pragma once

#include "Memory/LIWMemory.h"
#include "LIWFrame.h"
#include "Threading/LIWFiberTask.h"
#include "Threading/LIWFiberExecutor.h"
#include "Threading/LIWThreadWorkerTask.h"
#include "Threading/LIWMainThreadExecutor.h"

#include "Framework/LIWECSFunctional.h"

namespace LIW {
	namespace Editor {
		/// <summary>
		/// Begin drawing editor UI
		/// </summary>
		class LIW_FT_EDTR_UIDrawBeg final :
			public LIWFiberTask
		{
		public:
			void Execute(LIWFiberWorkerPointer thisFiber) override;
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
			void Execute(LIWFiberWorkerPointer thisFiber) override;
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
			void Execute(LIWFiberWorkerPointer thisFiber) override;
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


		void DrawSceneGraphUI();

		class LIWTEST_TT_TestDrawUI final :
			public LIWThreadWorkerTask
		{
		public:
			void Execute() override;
			LIWPointer<LIWFrameData, LIWMem_Frame> ptrFrameData;
		};
	}
}

