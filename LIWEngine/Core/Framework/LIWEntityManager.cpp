#include "LIWEntityManager.h"

LIW::LIWEntityManager::LIWEntityManager(uint32_t countEntity):
	m_entityCapacity(countEntity)
{
	m_entities.resize(countEntity);
	uint32_t i = 0;
	for (auto itr = m_entities.begin(); itr != m_entities.end(); itr++, i++) {
		itr->m_handle = i;
	}
}

LIW::LIWEntityManager::~LIWEntityManager() {

}