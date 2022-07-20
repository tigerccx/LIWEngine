#pragma once
#include <memory>
#include <list>
#include <mutex>

#include "LIWEntity.h"
#include "Containers/LIWDArray.h"

#define LIW_ENTITY_COUNT uint32_t(1) << 20 // 1M entities

namespace LIW {
	//TODO: Create container and use inhouse mem management
	class LIWEntityManager final{
	public:
		typedef std::mutex mtx_type;
		typedef std::lock_guard<mtx_type> lkgd_type;
	public:
		LIWEntityManager(uint32_t countEntity = LIW_ENTITY_COUNT);
		~LIWEntityManager();

		inline LIWEntity FetchEntity() {
			if (m_entitiesAvailable.empty())
				throw "Entities exhausted. ";
			LIWEntity entity = m_entitiesAvailable.front();
			m_entitiesAvailable.pop_front();
			return entity;
		}
		inline LIWEntity FetchEntity_thdsf() {
			lkgd_type lk(m_mtx);
			return FetchEntity();
		}
		inline void FetchEntities(LIWDArray<LIWEntity>& entities, uint32_t count) {
			if (m_entitiesAvailable.size() < count)
				throw "Entities exhausted. ";
			entities.set_capacity(count);
			for (uint32_t i = 0; i < count; i++) {
				entities.push_back(m_entitiesAvailable.front());
				m_entitiesAvailable.pop_front();
			}
		}
		inline void FetchEntities_thdsf(LIWDArray<LIWEntity>& entities, uint32_t count) {
			lkgd_type lk(m_mtx);
			FetchEntities(entities, count);
		}
		inline void ReturnEntity(LIWEntity entity) {
			m_entitiesAvailable.push_front(entity);
		}
		inline void ReturnEntity_thdsf(LIWEntity entity) {
			lkgd_type lk(m_mtx);
			ReturnEntity(entity);
		}
	private:
		mtx_type m_mtx;
		uint32_t m_entityCapacity{ 0 };
		std::list<LIWEntity> m_entitiesAvailable;
	};
}