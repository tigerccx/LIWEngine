#pragma once
#include "Components/LIWComponent_SceneNode.h"
#include "LIWEntity.h"

namespace LIW {
	class LIWSceneGraph {
	public:
		void SetParent(liw_objhdl_type hdlChild, liw_objhdl_type hdlParent);
		void UnsetParent(liw_objhdl_type hdlChild);

		LIWEntity GetEntityParent(LIWEntity entity);
		void SetEntityParent(LIWEntity entityChild, LIWEntity entityParent);
		void UnsetEntityParent(LIWEntity entityChild);
	};
}