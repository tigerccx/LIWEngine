#pragma once

#include "LIWComponent.h"
#include "LIWComponentManager.h"

#include "Components/LIWComponent_Transform.h"

#define LIW__COMPONENT_MANAGER_NAME(ComponentType) m__componentManager_##ComponentType
#define LIW_INJECT_COMPONENT_MANAGER(ComponentType, initCapacity, expandCount) LIWComponentManager<ComponentType> LIW__COMPONENT_MANAGER_NAME(ComponentType){initCapacity, expandCount};

namespace LIW {
	/// <summary>
	/// Contains builtin component managers. 
	/// Custom component collection should inherit from this struct. 
	/// </summary>
	struct LIWComponentCollection {
		LIW_INJECT_COMPONENT_MANAGER(LIWComponent_Transform, 1 << 15, 1 << 15);
	};
}