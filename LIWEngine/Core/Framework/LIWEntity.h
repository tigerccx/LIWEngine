#pragma once
#include <vector>
#include <list>
#include <algorithm>
#include <string>

#include "LIWObject.h"
#include "LIWComponent.h"

namespace LIW {

	/*
	* Class: LIWEntity
	* Author: tigerccx
	* Description:	This class is used as a baseclass Entity in a ECS framework.
	*				A LIWEntity in LIW framework is only represented as a handle. 
	*/
	class LIWEntity final :
		public LIWObject
	{
		friend class LIWEntityManager;
	public:
		typedef uint32_t handle_type;
		static const handle_type sc_hdlnull;
	public:
		LIWEntity();
		~LIWEntity() = default;
	private:
		handle_type m_handle;
	};
	//public:
	//	friend class LIWEntityManager;

	//	/*Component Management*/
	//	/*
	//	* Func: AddComponent
	//	* Description: Used to add Component to this Entity.
	//	*/
	//	void AddComponent(LIWComponent* component, bool toActivate=true);
	//	/*
	//	* Func: GetComponent
	//	* Description: Used to get (the first) type T Component from this Entity.
	//	*/
	//	template<class T> T* GetComponent() const;
	//	/*
	//	* Func: GetComponents
	//	* Description: Used to get all the type T Component from this Entity.
	//	*/
	//	template<class T> void GetComponents(std::vector<T*>& components) const;
	//	/*
	//	* Func: RemoveComponent
	//	* Description: Used to remove a Component from this Entity.
	//	*/
	//	void RemoveComponent(LIWComponent* component);
	//	/*
	//	* Func: RemoveComponent
	//	* Description: Used to remove (the first) type T Component from this Entity.
	//	*/
	//	template<class T> T* RemoveComponent();
	//	/*
	//	* Func: RemoveComponents
	//	* Description: Used to remove all the type T Components from this Entity.
	//	*/
	//	template<class T> void RemoveComponents(std::vector<T*>& components);
	//	/*
	//	* Func: GetComponentList
	//	* Description: Get list of all components attached to this entity.
	//	*/
	//	inline const std::vector<LIWComponent*>& GetComponentList() const { return m_components; }

	//	/*
	//	* Func: SetName
	//	* Description: Set the name of Entity.
	//	*/
	//	inline void SetName(const std::string& name) { m_name = name; }
	//	/*
	//	* Func: GetName
	//	* Description: Get the name of Entity.
	//	*/
	//	inline std::string GetName() { return m_name; }
	//protected:
	//	LIWEntity() : LIWObject() {}
	//	virtual ~LIWEntity() {}

	//	std::vector<LIWComponent*> m_components;
	//	std::string m_name = "LIWEntity";
	//};

	//template<class T>
	//inline T* LIWEntity::GetComponent() const
	//{
	//	LIWTypeID idType = GetTypeID<T>();
	//	for (auto itr : m_components) {
	//		if (itr->GetObjectTypeID() == idType) {
	//			return (T*)itr;
	//		}
	//	}
	//}
	//template<class T>
	//inline void LIWEntity::GetComponents(std::vector<T*>& components) const
	//{
	//	LIWTypeID idType = GetTypeID<T>();
	//	for (auto itr : m_components) {
	//		if (itr->GetObjectTypeID() == idType) {
	//			components.emplace_back(*itr);
	//		}
	//	}
	//}
	//template<class T>
	//inline T* LIWEntity::RemoveComponent()
	//{
	//	LIWTypeID idType = GetTypeID<T>();
	//	LIWComponent* componentRemove = nullptr;
	//	for (auto itr : m_components) {
	//		if (itr->GetObjectTypeID() == idType) {
	//			componentRemove = *itr;
	//			componentRemove->SetEntity(nullptr);
	//			m_components.erase(itr);
	//			break;
	//		}
	//	}
	//	return componentRemove;
	//}
	//template<class T>
	//inline void LIWEntity::RemoveComponents(std::vector<T*>& components)
	//{
	//	LIWTypeID idType = GetTypeID<T>();
	//	for (auto itr = m_components.begin(); itr != m_components.end(); ++itr) {
	//		if (itr->GetObjectType() == idType) {
	//			(*itr)->SetEntity(nullptr);
	//			components.emplace_back(*itr);
	//			itr = m_components.erase(itr);
	//		}
	//	}
	//}
}