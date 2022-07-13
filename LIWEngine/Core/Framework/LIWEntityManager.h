#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <assert.h>

#include "LIWObject.h"
#include "LIWEntity.h"

namespace LIW {
	/*
	* Class: LIWEntityManager
	* Author: tigerccx
	* Description:	Manager of all the LIWEntities.
	*/
	class LIWEntityManager :
		public LIWObject
	{
	public:
		static LIWEntityManager instance;

		/*
		* Func: GetNewEntity
		* Description:	Get a new Entity.
		*/
		LIWEntity* GetNewEntity() {
			return CreateEntity();
		}

		/*
		* Func: ReturnEntity
		* Description:	Return an Entity.
		*				Note: Returned Entity should be considered unusable.
		*/
		void ReturnEntity(LIWEntity* entity) {
			DeleteEntity(entity);
		}

		/*
		* Func: GetEntityList
		* Description:	Return a vector containing all Entities.
		*				Note: Vector got could be changed!
		*/
		const std::vector<LIWEntity*>& GetEntityList() const {
			return m_entities;
		}


	protected:
		std::vector<LIWEntity*> m_entities;
		
		LIWEntityManager() = default;
		~LIWEntityManager() = default;

		LIWEntity* CreateEntity() {
			LIWEntity* entity = new LIWEntity();
			m_entities.emplace_back(entity);
			return entity;
		}

		bool DeleteEntity(LIWEntity* entity) {
			for (auto itr = m_entities.begin(); itr != m_entities.end(); ++itr) {
				if (*itr == entity) {
					delete entity;
					m_entities.erase(itr);
					break;
				}
			}
		}
	};
}