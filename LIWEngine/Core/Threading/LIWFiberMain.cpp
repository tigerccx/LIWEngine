#include "LIWFiberMain.h"
#include "LIWFiberWorker.h"

//
// Win32
//
#ifdef _WIN32
void LIW::LIWFiberMain::YieldTo(LIWFiberWorkerPointer fiberOther)
{
	fiberOther->record.fetch_add(1);
	SwitchToFiber(fiberOther->m_sysFiber);
}
LIW::LIWFiberMain::LIWFiberMain(int threadID):
	m_threadID(threadID)
{
	m_sysFiber = ConvertThreadToFiber(nullptr);
}
LIW::LIWFiberMain::~LIWFiberMain()
{
}
#endif