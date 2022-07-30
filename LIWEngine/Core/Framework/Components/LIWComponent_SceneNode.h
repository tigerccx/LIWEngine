#pragma once
#include "Maths/LIWMaths.h"
#include "Containers/LIWDArray.h"
#include "Framework/LIWComponent.h"

//TODO: Make LIWSet container and replace LIWDArray with that.

namespace LIW {
	class LIWComponent_SceneNode final: public LIWComponent {
	public:
		liw_objhdl_type m_parent{ liw_c_nullobjhdl };
		LIWDArray<liw_objhdl_type, LIWMem_Default> m_children{0};
	};
}