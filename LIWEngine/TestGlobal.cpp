#include "TestGlobal.h"

LIWPointer<TestRenderer, LIWMem_Static> TestGlobal::s_rendererTest{ liw_c_nullhdl };
LIWPointer<LIW::OGLRenderer_Forward, LIWMem_Static> TestGlobal::s_rendererForward{ liw_c_nullhdl };
LIWPointer<LIW::OGLRenderer_Deferred, LIWMem_Static> TestGlobal::s_rendererDeferred{ liw_c_nullhdl };
LIWPointer<LIW::OGLRenderer_DeferredVisibility, LIWMem_Static> TestGlobal::s_rendererVisibility{ liw_c_nullhdl };
liw_objhdl_type TestGlobal::cam{ liw_c_nullobjhdl };