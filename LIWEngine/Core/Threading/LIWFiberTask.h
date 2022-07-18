#pragma once
#include "LIWFiberCommon.h"
//#include "LIWFiberWorker.h"

namespace LIW {
	class LIWFiberWorker;
	class LIWFiberTask {
	public:
		virtual void Execute(LIWFiberWorkerPointer thisFiber) = 0;
	};
}