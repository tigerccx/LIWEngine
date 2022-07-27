#pragma once
#include <stdexcept>

#include "Framework/LIWSystem.h"
#include "Framework/Components/LIWComponent_CameraController.h"
#include "Framework/LIWComponentManager.h"
#include "Memory/LIWMemory.h"
#include "Threading/LIWFiberExecutor.h"
#include "Threading/LIWFiberTask.h"
#include "LIWFrame.h"

namespace LIW {
	class LIWSystem_CameraControl final :
		public LIW::LIWSystem
	{

	};
}




class LIW_FT_LIWSystem_CameraControl_Update final :
	public LIW::LIWFiberTask
{
public:
	void Execute(LIW::LIWFiberWorkerPointer thisFiber) override;
//public:
//	LIWPointer<LIW::LIWFrameData, LIWMem_Frame> m_ptrFrameData{};
};