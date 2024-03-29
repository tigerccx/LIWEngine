#pragma once

#include "LIWComponent.h"
#include "LIWComponentManager.h"

#include "Components/LIWComponent_Transform.h"
#include "Components/LIWComponent_SceneNode.h"
#include "Components/LIWComponent_MeshRenderer.h"
#include "Components/LIWComponent_MeshRendererBatched.h"
#include "Components/LIWComponent_Camera.h"
#include "Components/LIWComponent_CameraController.h"
#include "Components/LIWComponent_Light.h"

#define LIW__COMPONENT_MANAGER_NAME(ComponentType) m__componentManager_##ComponentType
#define LIW_INJECT_COMPONENT_MANAGER(ComponentType, initCapacity, expandCount) LIWComponentManager<ComponentType> LIW__COMPONENT_MANAGER_NAME(ComponentType){initCapacity, expandCount};

namespace LIW {
	/// <summary>
	/// Contains builtin component managers. 
	/// Custom component collection should inherit from this struct. 
	/// </summary>
	struct LIWComponentCollection {
		LIW_INJECT_COMPONENT_MANAGER(LIWComponent_Transform, 1 << 10, 1 << 10);
		LIW_INJECT_COMPONENT_MANAGER(LIWComponent_SceneNode, 1 << 10, 1 << 10);
		LIW_INJECT_COMPONENT_MANAGER(LIWComponent_MeshRenderer, 1 << 10, 1 << 10);
		LIW_INJECT_COMPONENT_MANAGER(LIWComponent_MeshRendererBatched, 1 << 10, 1 << 10);
		LIW_INJECT_COMPONENT_MANAGER(LIWComponent_Camera, 4, 2);
		LIW_INJECT_COMPONENT_MANAGER(LIWComponent_CameraController, 4, 2);
		LIW_INJECT_COMPONENT_MANAGER(LIWComponent_Light, 16, 8);
	};
}