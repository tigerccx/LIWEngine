#include "LIWConfig.h"
#include "LIWEntityManager.h"

LIW::LIWEntityManager::LIWEntityManager(uint32_t countEntity):
	m_entityCapacity(countEntity)
{
	m_entitiesAvailable.resize(countEntity);
	uint32_t i = 0;
	for (auto itr = m_entitiesAvailable.begin(); itr != m_entitiesAvailable.end(); itr++, i++) {
		*itr = i;
	}
}

LIW::LIWEntityManager::~LIWEntityManager() {
#ifdef LIW_DEBUG
	if (m_entitiesAvailable.size() != m_entityCapacity)
		printf("Warning: Not all entities have been returned. \n");
#endif
}