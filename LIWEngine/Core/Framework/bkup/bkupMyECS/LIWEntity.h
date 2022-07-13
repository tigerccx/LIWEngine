#pragma once
#include <vector>
#include <algorithm>

#include "LIWObject.h"
#include "LIWComponent.h"
#include "LIWModel.h"

namespace LIW {
	/*
	* Class: LIWEntity
	* Author: tigerccx
	* Description:	This class is used as a baseclass Entity in a ECS framework.
	*				A LIWEntity in LIW framework contains multiple components and multiple models,
	*				where components serve as functional plugins and models serve as data containers.
	*/
	class LIWEntity:
		public LIWObject
	{
	public:
		LIWEntity() : LIWObject() {}
		virtual ~LIWEntity() {}


		/*Component Management*/
		/*
		* Func: AddComponent
		* Description: Used to add Component to this Entity.
		*/
		void AddComponent(LIWComponent* component, bool toActivate=true);
		/*
		* Func: GetComponent
		* Description: Used to get (the first) type T Component from this Entity.
		*/
		template<class T> T* GetComponent();
		/*
		* Func: GetComponents
		* Description: Used to get all the type T Component from this Entity.
		*/
		template<class T> void GetComponents(std::vector<T*>& components);
		/*
		* Func: RemoveComponent
		* Description: Used to remove a Component from this Entity.
		*/
		void RemoveComponent(LIWComponent* component);
		/*
		* Func: RemoveComponent
		* Description: Used to remove (the first) type T Component from this Entity.
		*/
		template<class T> T* RemoveComponent();
		/*
		* Func: RemoveComponents
		* Description: Used to remove all the type T Components from this Entity.
		*/
		template<class T> void RemoveComponents(std::vector<T*>& components);


		/*Model Management*/
		/*
		* Func: AddModel
		* Description: Used to add Model to this Entity.
		*/
		void AddModel(LIWModel* model);
		/*
		* Func: GetModel
		* Description: Used to get (the first) type T Model from this Entity.
		*/
		template<class T> T* GetModel();
		/*
		* Func: GetModels
		* Description: Used to get all the type T Models from this Entity.
		*/
		template<class T> void GetModels(std::vector<T*>& models);
		/*
		* Func: RemoveModel
		* Description: Used to remove a Model from this Entity.
		*/
		void RemoveModel(LIWModel* model);
		/*
		* Func: RemoveModel
		* Description: Used to remove (the first) type T Model from this Entity.
		*/
		template<class T> T* RemoveModel();
		/*
		* Func: RemoveModels
		* Description: Used to remove all the type T Models from this Entity.
		*/
		template<class T> void RemoveModels(std::vector<T*>& models);

	protected:
		std::vector<LIWComponent*> m_components;
		std::vector<LIWModel*> m_models;
	};
	template<class T>
	inline T* LIWEntity::GetComponent()
	{
		LIWTypeID idType = GetType<T>();
		for (auto itr : m_components) {
			if (itr->GetObjectType() == idType) {
				return (T*)itr;
			}
		}
	}
	template<class T>
	inline void LIWEntity::GetComponents(std::vector<T*>& components)
	{
		LIWTypeID idType = GetType<T>();
		for (auto itr : m_components) {
			if (itr->GetObjectType() == idType) {
				components.emplace_back(*itr);
			}
		}
	}
	template<class T>
	inline T* LIWEntity::RemoveComponent()
	{
		LIWTypeID idType = GetType<T>();
		LIWComponent* componentRemove = nullptr;
		for (auto itr : m_components) {
			if (itr->GetObjectType() == idType) {
				componentRemove = *itr;
				componentRemove->SetEntity(nullptr);
				m_components.erase(itr);
				break;
			}
		}
		return componentRemove;
	}
	template<class T>
	inline void LIWEntity::RemoveComponents(std::vector<T*>& components)
	{
		LIWTypeID idType = GetType<T>();
		for (auto itr = m_components.begin(); itr != m_components.end(); ++itr) {
			if (itr->GetObjectType() == idType) {
				(*itr)->SetEntity(nullptr);
				components.emplace_back(*itr);
				itr = m_components.erase(itr);
			}
		}
	}

	template<class T>
	inline T* LIWEntity::GetModel()
	{
		LIWTypeID idType = GetType<T>();
		for (auto itr : m_models) {
			if (itr->GetObjectType() == idType) {
				return (T*)itr;
			}
		}
	}
	template<class T>
	inline void LIWEntity::GetModels(std::vector<T*>& models)
	{
		LIWTypeID idType = GetType<T>();
		for (auto itr : m_models) {
			if (itr->GetObjectType() == idType) {
				models.emplace_back(*itr);
			}
		}
	}
	template<class T>
	inline T* LIWEntity::RemoveModel()
	{
		LIWTypeID idType = GetType<T>();
		LIWModel* modelRemove = nullptr;
		for (auto itr : m_models) {
			if (itr->GetObjectType() == idType) {
				modelRemove = *itr;
				modelRemove->SetEntity(nullptr);
				m_models.erase(itr);
				break;
			}
		}
		return modelRemove;
	}
	template<class T>
	inline void LIWEntity::RemoveModels(std::vector<T*>& models)
	{
		LIWTypeID idType = GetType<T>();
		for (auto itr = m_models.begin(); itr != m_models.end(); ++itr) {
			if (itr->GetObjectType() == idType) {
				(*itr)->SetEntity(nullptr);
				models.emplace_back(*itr);
				itr = m_models.erase(itr);
			}
		}
	}
}