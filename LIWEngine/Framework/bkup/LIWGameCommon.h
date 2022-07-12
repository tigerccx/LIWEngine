#pragma once
#include "LIWGame.h"
#include "LIWComponentManager.h"

namespace LIW {
	class LIWGameCommon :
		public LIWGame
	{
	public:
		LIWGameCommon() = default;
		virtual ~LIWGameCommon() = default;

		virtual int Initialise() override;
		virtual int UpdateMainLoop() override;
		virtual int CleanUp() override;
	};
}



