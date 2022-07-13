#pragma once
#include "LIWCore.h"

int run(void)
{
	using namespace LIW;
	using namespace std::chrono;

	LIWCore::s_ins.Boot();

	//std::this_thread::sleep_for(1s);
	LIWCore::s_ins.MainThreadLoop();

	LIWCore::s_ins.Shutdown();


	return 0;
}