#pragma once
#include "LIWTypes.h"
//#include "Maths/LIWMaths.h"
#include "Framework/LIWComponent.h"
#include "Data/LIWMesh.h"
#include "Data/LIWMaterial.h"

namespace LIW {
	class LIWComponent_MeshRenderer final: public LIWComponent {
	public:
		LIWPointer<LIWMaterial, LIWMem_Default> m_handleMaterial;
		LIWPointer<LIWMesh, LIWMem_Default> m_handleMesh;
	};
}