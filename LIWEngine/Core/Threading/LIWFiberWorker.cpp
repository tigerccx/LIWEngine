#include "LIWFiberWorker.h"
#include "LIWFiberMain.h"

//
// Win32
//
#ifdef _WIN32
LIW::LIWFiberWorker::LIWFiberWorker(): LIWFiberWorker(-1)
{
	
}
LIW::LIWFiberWorker::LIWFiberWorker(int id):
	m_id(id),
	m_isRunning(true)
{
	m_sysFiber = CreateFiber(1024*64, InternalFiberRun, this);
	if (!m_sysFiber)
		throw "Fiber creation failed.";
}
LIW::LIWFiberWorker::~LIWFiberWorker()
{
	DeleteFiber(m_sysFiber);
}
void LIW::LIWFiberWorker::Run()
{
	while (m_isRunning) {
		m_state = LIWFiberState::Running;
		auto ptrTask = m_curTask.get_ptr();
		ptrTask->Execute(LIWFiberWorkerPointer((liw_hdl_type)this));
		m_state = LIWFiberState::Idle;
		liw_delete_def<LIWFiberTask>(m_curTask); // Delete task, since everything is done.
		m_curTask.set_null();
		//printf("%p from fb%d thd%d\n", m_curTask, m_id, m_fiberMain->m_threadID);
		YieldToMain();
	}
}
inline void LIW::LIWFiberWorker::YieldToMain()
{
	record.fetch_sub(1);
	LPVOID mainFiber = m_fiberMain->m_sysFiber;
	m_fiberMain = nullptr;
	SwitchToFiber(mainFiber);
}
#endif 


