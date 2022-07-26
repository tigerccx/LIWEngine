#include "LIWGlobal.h"

namespace LIW {
	LIWPointer<LIWGame, LIWMem_Static> LIWGlobal::s_game;
	LIWPointer<LIWAssetManager, LIWMem_Static> LIWGlobal::s_assetManager;
	LIWPointer<App::Environment, LIWMem_Static> LIWGlobal::s_environment;
}