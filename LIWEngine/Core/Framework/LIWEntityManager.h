#pragma once
#include <memory>
#include <list>

#include "LIWEntity.h"

namespace LIW {
	//TODO: Create container and use inhouse mem management
	class LIWEntityManager final{
	public:
		LIWEntityManager(uint32_t countEntity); 
		~LIWEntityManager();

		inline LIWEntity GetEntity() {
			LIWEntity entity = m_entities.front();
			return std::move(entity);
		}
		inline void ReturnEntity(LIWEntity&& entity) {
			m_entities.emplace_front(entity);
		}
	private:
		std::list<LIWEntity> m_entities;
		uint32_t m_entityCapacity{ 0 };
	};
}