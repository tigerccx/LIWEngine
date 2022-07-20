#pragma once

#include "Memory/LIWMemory.h"
#include "Framework/LIWGame.h"
#include "Application/Environment.h"

namespace LIW {
	class LIWGlobal {
		friend class LIWCore;
	public:
		static inline LIWPointer<LIWGame, LIWMem_Static> GetGame() {
			return s_game;
		}
		static inline LIWPointer<App::Environment, LIWMem_Static> GetEnvironment() {
			return s_environment;
		}
	private:
		static LIWPointer<LIWGame, LIWMem_Static> s_game;
		static LIWPointer<App::Environment, LIWMem_Static> s_environment;
	};
}