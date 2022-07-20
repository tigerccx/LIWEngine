#include "LIWGlobal.h"

namespace LIW {
	LIWPointer<LIWGame, LIWMem_Static> LIWGlobal::s_game;
	LIWPointer<App::Environment, LIWMem_Static> LIWGlobal::s_environment;
}