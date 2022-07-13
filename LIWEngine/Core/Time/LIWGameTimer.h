/******************************************************************************
Class:GameTimer
Author:Rich Davison
Description:Wraps Windows PerformanceCounter. GameTimers keep track of how much
time has passed since they were last polled - so you could use multiple
GameTimers to trigger events at different time periods. 

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
/*
* Revision
* Author: tigerccx
* Description: Only changed the name and baseclass to integrate into the framework. 
*/

#pragma once
#include <chrono>

#include "LIWObject.h"

namespace LIW{
	class LIWGameTimer:
		public LIWObject
	{
	public:
		LIWGameTimer(void);
		virtual ~LIWGameTimer(void) {}

		double	GetTotalTimeSeconds()	const;
		double	GetTotalTimeMSec()		const;

		float	GetTimeDeltaSeconds()	const { return timeDelta; };
		float	GetTimeDeltaMSec()		const { return timeDelta * 1000.0f; };

		void	Tick();
	protected:
		typedef  std::chrono::time_point<std::chrono::high_resolution_clock>  Timepoint;
		float		timeDelta;
		Timepoint	firstPoint;
		Timepoint	nowPoint;
	};
}



