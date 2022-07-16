#pragma once
#include "LIWSystem.h"

namespace LIW {
	class LIWStandardSystem:
		public LIWSystem
	{
	public:
		LIWStandardSystem() = default;
		virtual ~LIWStandardSystem() = default;

		virtual void Init() { }
		virtual void Cleanup() { }


		/*
		* Func: PreFrame
		* Description:	Called before frame update.
		*/
		virtual void PreFrame(float dt) { }
		/*
		* Func: Update
		* Desciption:	Update.
		*/
		virtual void Update(float dt) { }
		/*
		* Func: FixedUpdate
		* Desciption:	Update called at a fixed rate.
		*/
		virtual void FixedUpdate(float dt) { }
		/*
		* Func: PreRenderUpdate
		* Description:	Update before render.
		*/
		virtual void PreRenderUpdate(float dt) { }
		/*
		* Func: RenderUpdate
		* Description:	RenderUpdate.
		*/
		virtual void RenderUpdate(float dt) { }
		/*
		* Func: PostRenderUpdate
		* Description:	Update after render.
		*/
		virtual void PostRenderUpdate(float dt) { }
		/*
		* Func: PostFrame
		* Description:	Called at the end of frame update.
		*/
		virtual void PostFrame(float dt) { }
	};
}

#include "Memory/LIWMemory.h"
#include "Fiber/LIWFiber.h"
#include "LIWFrame.h"

struct StandardSystemData {
	LIWPointer<LIWFrameData, LIWMem_Frame> m_hdlFrameData;
	LIW::LIWStandardSystem* m_system;
};

void FT_SystemUpdate(LIW_FIBER_RUNNER_PARAM);
void FT_SystemRenderUpdate(LIW_FIBER_RUNNER_PARAM);