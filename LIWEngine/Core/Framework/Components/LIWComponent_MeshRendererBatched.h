#pragma once
#include "LIWTypes.h"
//#include "Maths/LIWMaths.h"
#include "Framework/LIWComponent.h"
#include "Data/LIWMesh.h"
#include "Data/LIWMaterial.h"

namespace LIW {
	class LIWComponent_MeshRendererBatched final: public LIWComponent {
	public:
		LIWPointer<LIWMaterial, LIWMem_Default> m_handleMaterial;
		LIWPointer<LIWMeshData, LIWMem_Default> m_handleMeshData;
	};
}