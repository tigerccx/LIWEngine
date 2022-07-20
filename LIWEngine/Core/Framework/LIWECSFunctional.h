#pragma once
/*
* This header contains codes required for the ECS to work
* Note: only include this in .cpp file to avoid circular dependency
*/

#include "LIWEntity.h"
#include "LIWComponent.h"
#include "LIWComponentManager.h"
#include "LIWSystem.h"
#include "LIWECSConfiguration.h"
#include "LIWGlobal.h"

//
// Component Manager Ops
// 
// Get ComponentManager of type ComponentType
#define LIW_ECS_GetComponentManager(ComponentType) LIW::LIWGlobal::GetGame()->m__componentCollection.LIW__COMPONENT_MANAGER_NAME(ComponentType)
#define LIW_ECS_ApplyChangeOnComponentManager(ComponentType) LIW_ECS_GetComponentManager(ComponentType).ApplyChange()

//
// Component Ops
//
// Creation
#define LIW_ECS_CreateComponent(ComponentType) LIW_ECS_GetComponentManager(ComponentType).CreateComponent()
#define LIW_ECS_CreateComponent_thdsf(ComponentType) LIW_ECS_GetComponentManager(ComponentType).CreateComponent_thdsf()
#define LIW_ECS_CreateComponents(ComponentType, handlesOut, count) LIW_ECS_GetComponentManager(ComponentType).CreateComponents(handlesOut, count)
#define LIW_ECS_CreateComponents_thdsf(ComponentType, handlesOut, count) LIW_ECS_GetComponentManager(ComponentType).CreateComponents_thdsf(handlesOut, count)
// Get
#define LIW_ECS_GetComponent(ComponentType, handle) LIW_ECS_GetComponentManager(ComponentType).GetComponent(handle)
// Remove
// Removal will happen when LIW_ECS_ApplyChangeOnComponentManager(ComponentType) is called
#define LIW_ECS_RemoveComponent(ComponentType, handle) LIW_ECS_GetComponentManager(ComponentType).MarkRemove(handle)
// Activeness
// Change of activeness will happen when LIW_ECS_ApplyChangeOnComponentManager(ComponentType) is called
#define LIW_ECS_ActivateComponent(ComponentType, handle) LIW_ECS_GetComponentManager(ComponentType).MarkActive(handle)
#define LIW_ECS_DeactivateComponent(ComponentType, handle) LIW_ECS_GetComponentManager(ComponentType).MarkInactive(handle)

//
// Entity Manager Ops
// 
// Get EntityManager
#define LIW_ECS_GetEntityManager() LIW::LIWGlobal::GetGame()->m_entityManager

//
// Entity Ops
//
// Fetch
#define LIW_ECS_FetchEntity() LIW_ECS_GetEntityManager().FetchEntity()
#define LIW_ECS_FetchEntity_thdsf() LIW_ECS_GetEntityManager().FetchEntity_thdsf()
#define LIW_ECS_FetchEntities(entities, count) LIW_ECS_GetEntityManager().FetchEntities(entities, count)
#define LIW_ECS_FetchEntities_thdsf(entities, count) LIW_ECS_GetEntityManager().FetchEntities_thdsf(entities, count)
// Return
#define LIW_ECS_ReturnEntity(entity) LIW_ECS_GetEntityManager().ReturnEntity(entity)
// Component Relation
// Entity relation will happen when LIW_ECS_ApplyChangeOnComponentManager(ComponentType) is called
#define LIW_ECS_AttachComponentToEntity(ComponentType, handle, entity) LIW_ECS_GetComponentManager(ComponentType).MarkEntity(entity, handle)
#define LIW_ECS_DetachComponentToEntity(ComponentType, handle, entity) LIW_ECS_GetComponentManager(ComponentType).MarkRemoveEntity(entity, handle)
#define LIW_ECS_GetComponentFromEntity(ComponentType, entity) LIW_ECS_GetComponentManager(ComponentType).GetComponentFromEntity(entity)