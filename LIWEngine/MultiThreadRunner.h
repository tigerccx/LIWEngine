#pragma once
#include "LIWCore.h"

int run(void)
{
	using namespace LIW;
	using namespace std::chrono;

	LIWCore::s_ins.Boot();

	LIWCore::s_ins.MainThreadRun();

	LIWCore::s_ins.Shutdown();


	return 0;
}