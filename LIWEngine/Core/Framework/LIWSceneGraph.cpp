#include <cassert>
#include "LIWSceneGraph.h"
#include "LIWGlobal.h"
#include "Framework/LIWECSFunctional.h"

void LIW::LIWSceneGraph::SetParent(liw_objhdl_type hdlChild, liw_objhdl_type hdlParent)
{
	//auto& sceneNodeManager = LIW_ECS_GetComponentManager(LIWComponent_SceneNode);
	auto& sceneNodeChild = LIW_ECS_GetComponent(LIWComponent_SceneNode, hdlChild);
	auto& sceneNodeParent = LIW_ECS_GetComponent(LIWComponent_SceneNode, hdlParent);

	const liw_objhdl_type hdlOldParent = sceneNodeChild.m_parent;
	if (hdlOldParent != liw_c_nullobjhdl) { // Oh no! it has a parent! Quickly, disown each other! 
		auto& sceneNodeOldParent = LIW_ECS_GetComponent(LIWComponent_SceneNode, hdlOldParent);
		auto& children = sceneNodeOldParent.m_children;
		auto itr = children.get_beg();
		for (; itr != children.get_end(); itr++) {
			if (*itr == hdlChild) {
				children.erase(itr);
				break;
			}
		}
		assert(itr != children.get_end()); //Child and parent should be mutual!
	}
	sceneNodeParent.m_children.push_back(hdlChild);
	sceneNodeChild.m_parent = hdlParent;
}

void LIW::LIWSceneGraph::UnsetParent(liw_objhdl_type hdlChild)
{
	auto& sceneNodeChild = LIW_ECS_GetComponent(LIWComponent_SceneNode, hdlChild);
	const liw_objhdl_type hdlOldParent = sceneNodeChild.m_parent;
	if (hdlOldParent != liw_c_nullobjhdl) { // Oh no! it has a parent! Quickly, disown each other! 
		auto& sceneNodeOldParent = LIW_ECS_GetComponent(LIWComponent_SceneNode, hdlOldParent);
		auto& children = sceneNodeOldParent.m_children;
		auto itr = children.get_beg();
		for (; itr != children.get_end(); itr++) {
			if (*itr == hdlChild) {
				children.erase(itr);
				break;
			}
		}
		assert(itr != children.get_end()); //Child and parent should be mutual!
	}
	sceneNodeChild.m_parent = liw_c_nullobjhdl;
}

LIWEntity LIW::LIWSceneGraph::GetEntityParent(LIWEntity entity)
{
	liw_objhdl_type hdlChild = LIW_ECS_GetComponentFromEntity(LIWComponent_SceneNode, entity);
	auto& sceneNodeChild = LIW_ECS_GetComponent(LIWComponent_SceneNode, hdlChild);
	const liw_objhdl_type hdlParent = sceneNodeChild.m_parent;
	if (hdlParent == liw_c_nullobjhdl)
		return liw_c_nullobjhdl;
	auto& sceneNodeParent = LIW_ECS_GetComponent(LIWComponent_SceneNode, hdlChild);
	return sceneNodeParent.GetEntity();
}

void LIW::LIWSceneGraph::SetEntityParent(LIWEntity entityChild, LIWEntity entityParent)
{
	liw_objhdl_type hdlSceneNodeChild = LIW_ECS_GetComponentFromEntity(LIWComponent_SceneNode, entityChild);
	if (hdlSceneNodeChild == liw_c_nullobjhdl)
		throw std::runtime_error("this entity has no scene node. \n");
	liw_objhdl_type hdlSceneNodeParent = LIW_ECS_GetComponentFromEntity(LIWComponent_SceneNode, entityParent);
	if (hdlSceneNodeParent == liw_c_nullobjhdl)
		throw std::runtime_error("this entity has no scene node. \n");
	SetParent(hdlSceneNodeChild, hdlSceneNodeParent);
}

void LIW::LIWSceneGraph::UnsetEntityParent(LIWEntity entityChild)
{
	liw_objhdl_type hdlSceneNodeChild = LIW_ECS_GetComponentFromEntity(LIWComponent_SceneNode, entityChild);
	if (hdlSceneNodeChild == liw_c_nullobjhdl)
		throw std::runtime_error("this entity has no scene node. \n");
	UnsetParent(hdlSceneNodeChild);
}
