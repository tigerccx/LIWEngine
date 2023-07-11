#include "LIWGlobal.h"

namespace LIW {
	LIWIDManager LIWGlobal::s_idManager;

	LIWPointer<LIWGame, LIWMem_Static> LIWGlobal::s_game;
	LIWPointer<LIWAssetManager<LIWMem_Default>, LIWMem_Static> LIWGlobal::s_assetManager;
	LIWPointer<App::Environment, LIWMem_Static> LIWGlobal::s_environment;
}