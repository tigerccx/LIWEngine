#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <assert.h>

#include "LIWObject.h"
#include "LIWComponent.h"

namespace LIW {
	/*
	* Class: LIWComponentManager
	* Author: tigerccx
	* Description:	Manager of all the T type LIWComponents. 
	*/
	template<class T>
	class LIWComponentManager: 
		public LIWObject
	{
	public:
		typedef void(*FuncInitialise)(T* component, void* param);
		typedef void(*FuncCleanup)(T* component);
		
		static LIWComponentManager<T> instance;

		FuncInitialise m_funcInit = nullptr;
		FuncCleanup m_funcCleanup = nullptr;

		/*
		* Func: GetNewComponent
		* Description:	Get a new type T Component.
		*				Note: only get Component with LIWComponentManager. 
		*/
		T* GetNewComponent(void* param = 0) {
			return CreateComponent(param);
		}

		/*
		* Func: ReturnComponent
		* Description:	Return a type T Component.
		*				Note: Returned Component should be considered unusable.
		*/
		void ReturnComponent(T* component) {
			DeleteComponent(component);
		}

		/*
		* Func: GetComponentList
		* Description:	Return a vector containing all Components.
		* 				Note: Vector got could be changed!
		*/
		const std::vector<T*>& GetComponentList() const {
			return m_components;
		}

	protected:		
		std::vector<T*> m_components;
		//std::vector<T*>
		//std::map<int, std::vector<LIWComponent*>*> m_componentsPriority;

		LIWComponentManager<T>() = default;
		~LIWComponentManager<T>() = default;

		T* CreateComponent(void* param = 0) {
			T* component = new T();
			if (m_funcInit)
				m_funcInit(component, param);
			m_components.emplace_back(component);
			return component;
		}

		bool DeleteComponent(T* component) {
			for (auto itr : m_components) {
				if ((*itr) == component) {
					if(m_funcCleanup)
						m_funcCleanup(component);
					delete component;
					m_components.erase(itr);
					break;
				}
			}
		}

		///*
		//* Func: RegisterPriority
		//* Description:	Register component to m_componentsPriority according to its priority.
		//*/
		//void RegisterPriority(LIWComponent* component, int priority) {
		//	auto itr = m_componentsPriority.find(priority);
		//	if (itr == m_componentsPriority.end()) {
		//		m_componentsPriority[priority] = new std::vector<LIWComponent*>();
		//	}
		//	m_componentsPriority[priority]->emplace_back(component);
		//}
		///*
		//* Func: UnregisterPriority
		//* Description:	Unregister component to m_componentsPriority according to its priority.
		//*/
		//void UnregisterPriority(LIWComponent* component, int priority) {
		//	auto components = m_componentsPriority[priority];
		//	assert(components != nullptr);
		//	auto itr = std::find(components->begin(), components->end(), component);
		//	assert(itr != components->end());
		//	components->erase(itr);
		//}
	};

	template<class T> LIWComponentManager<T> LIWComponentManager<T>::instance;
}