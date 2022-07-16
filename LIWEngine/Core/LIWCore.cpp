#include "LIWCore.h"

LIW::LIWCore LIW::LIWCore::s_ins;

void LIW::LIW_FT_NotifyStop::Execute(LIWFiberWorker* thisFiber)
{
	LIWCore::s_ins.NotifyShutdown();
}
