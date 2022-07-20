#pragma once

#include "LIWComponent.h"
#include "LIWComponentManager.h"

#define LIW__COMPONENT_MANAGER_NAME(ComponentType) m__componentManager_##ComponentType
#define LIW_INJECT_COMPONENT_MANAGER(ComponentType, initCapacity, expandCount) LIWComponentManager<ComponentType> LIW__COMPONENT_MANAGER_NAME(ComponentType){initCapacity, expandCount};

namespace LIW {
	/// <summary>
	/// Contains builtin component managers. 
	/// Custom component collection should inherit from this struct. 
	/// </summary>
	struct LIWComponentCollection {

	};
}