#include "TestGlobal.h"

LIWPointer<LIW::OGLRendererForward, LIWMem_Static> TestGlobal::s_renderer{ liw_c_nullhdl };
liw_objhdl_type TestGlobal::cam{ liw_c_nullobjhdl };