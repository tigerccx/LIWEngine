#pragma once
#include "LIWTypes.h"
//#include "Maths/LIWMaths.h"
#include "Framework/LIWComponent.h"
#include "Data/LIWMesh.h"
#include "Data/LIWMaterial.h"

namespace LIW {
	class LIWComponent_MeshRenderer :public LIWComponent {
	public:
		liw_objhdl_type m_handleMaterial;
		liw_objhdl_type m_handleMesh;
	};
}