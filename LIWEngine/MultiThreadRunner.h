#pragma once
#include "LIWCore.h"

int run(void)
{
	using namespace LIW;
	using namespace std::chrono;

	int rslt = LIWCore::s_ins.Boot();

	LIWCore::s_ins.MainThreadRun();
	if (!rslt) {
		LIWCore::s_ins.MainThreadRun();
	}

	LIWCore::s_ins.Shutdown();


	return 0;
}