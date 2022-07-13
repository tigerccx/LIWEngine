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
	* Description:	Manager of LIWComponent. 
	*				Used to call all the existing components.
	*/
	class LIWComponentManager: 
		public LIWObject
	{
	public:
		LIWComponentManager() = default;
		~LIWComponentManager() = default;

		//static LIWComponentManager instance;
		/*
		* Func: RegisterComponent
		* Description:	Used to register new components to manager. 
		*				Note: A component needs to register itself to the manager!
		*/
		void RegisterComponent(LIWComponent* component) {
			m_components.emplace_back(component);
		}
		/*
		* Func: UnregisterComponent
		* Description:	Used to register new components to manager.
		*				Note: A component needs to register itself to the manager!
		*/
		void UnregisterComponent(LIWComponent* component) {
			auto itr = std::find(m_components.begin(), m_components.end(), component);
			if(itr!=m_components.end())
				m_components.erase(itr);
		}


		/*
		* Func: PreUpdate
		* Description:	Pre-update all components.
		*/
		void PreUpdate() {
			for (auto itr : m_components) {
				itr->PreUpdate();
			}
		}
		/*
		* Func: UpdateComponents
		* Desciption:	Update all active components.
		*/
		void UpdateComponents(float dt) {
			/*for (auto itr : m_components) {
				if(itr->IsActive())
					itr->Update(dt);
			}*/
			
			/*Update according to priority*/
			for (auto components : m_componentsPriority) {
				for (auto itr : (*(components.second))) {
					if (itr->IsActive())
						itr->Update(dt);
				}
			}
		}
		/*
		* Func: FixedUpdateComponents
		* Desciption:	Update all active components at a fixed rate.
		*/
		void FixedUpdateComponents(float dt) {
			for (auto itr : m_components) {
				if (itr->IsActive())
					itr->FixedUpdate(dt);
			}
		}
		/*
		* Func: PreRenderUpdate
		* Description:	Update all active components before render.
		*/
		void PreRenderUpdate() { 
			for (auto itr : m_components) {
				if (itr->IsActive())
					itr->PreRenderUpdate();
			}
		}
		/*
		* Func: PostRenderUpdate
		* Description:	Update all active components before render.
		*/
		void PostRenderUpdate() { 
			for (auto itr : m_components) {
				if (itr->IsActive())
					itr->PostRenderUpdate();
			}
		}

	protected:		
		std::vector<LIWComponent*> m_components;
		std::map<int, std::vector<LIWComponent*>*> m_componentsPriority;

		/*
		* Func: RegisterPriority
		* Description:	Register component to m_componentsPriority according to its priority.
		*/
		void RegisterPriority(LIWComponent* component, int priority) {
			auto itr = m_componentsPriority.find(priority);
			if (itr == m_componentsPriority.end()) {
				m_componentsPriority[priority] = new std::vector<LIWComponent*>();
			}
			m_componentsPriority[priority]->emplace_back(component);
		}
		/*
		* Func: UnregisterPriority
		* Description:	Unregister component to m_componentsPriority according to its priority.
		*/
		void UnregisterPriority(LIWComponent* component, int priority) {
			auto components = m_componentsPriority[priority];
			assert(components != nullptr);
			auto itr = std::find(components->begin(), components->end(), component);
			assert(itr != components->end());
			components->erase(itr);
		}
	};

	//LIWComponentManager LIWComponentManager::instance;
}