#pragma once

#include "Memory/LIWMemory.h"
#include "Framework/LIWGame.h"
#include "LIWIDManager.h"
#include "Data/LIWAssetManager.h"
#include "Application/Environment.h"

namespace LIW {
	class LIWGlobal {
		friend class LIWCore;
	public:
		static inline LIWPointer<LIWGame, LIWMem_Static> GetGame() {
			return s_game;
		}
		static inline LIWPointer<LIWAssetManager<LIWMem_Default>, LIWMem_Static> GetAssetManager() {
			return s_assetManager;
		}
		static inline LIWPointer<App::Environment, LIWMem_Static> GetEnvironment() {
			return s_environment;
		}
	public:
		static LIWIDManager s_idManager;

	private:
		static LIWPointer<LIWGame, LIWMem_Static> s_game;
		static LIWPointer<LIWAssetManager<LIWMem_Default>, LIWMem_Static> s_assetManager;
		static LIWPointer<App::Environment, LIWMem_Static> s_environment;
	};
}