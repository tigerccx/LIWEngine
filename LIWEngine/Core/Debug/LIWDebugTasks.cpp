#pragma once


#include "LIWDebugTasks.h"
#include "Threading/LIWThreadWorkerTask.h"

void LIW::LIW_TT_DebugPrint::Execute()
{
	m_ptrDebug->DebugPrint(m_line);
}

void LIW::LIW_TT_ErrorPrint::Execute()
{
	m_ptrDebug->ErrorPrint(m_line);
}
